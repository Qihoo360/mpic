
#include <evpp/exp.h>
#include <evpp/http/http_server.h>
#include <evpp/tcp_server.h>
#include <evpp/udp/udp_server.h>

#include "nfmpic_resource.h"

#include "../../nfmpic/option.h"

namespace nfmpic {

Resource::Resource() {
}

bool Resource::Init(const mpic::Option* mpic_op) {
    bool rc = true;
    const nfmpic::Option* op = dynamic_cast<const nfmpic::Option*>(mpic_op);
    assert(op);

    base_loop_ = std::make_shared<evpp::EventLoopThread>();

    if (!op->http_ports().empty()) {
        auto p = new evpp::http::Server();
        http_server_.reset(p);
        rc = http_server_->Init(op->http_ports());
        if (!rc) {
            LOG(ERROR) << "HTTP server start failed : "
                       << "http_ports=" << op->http_ports();
            return false;
        }
    }

    if (op->tcp_port() > 0) {
        std::string addr = std::string("0.0.0.0:") + std::to_string(op->tcp_port());
        auto p = new evpp::TCPServer(base_loop_->loop(),
                                     addr,
                                     "nfmpic-tcp",
                                     op->tcp_thread_pool_size());
        tcp_server_.reset(p);
        rc = tcp_server_->Init();
        if (!rc) {
            LOG(ERROR) << "TCP server start failed : "
                       << "tcp_port=" << op->tcp_port();
            return false;
        }
    }

    if (!op->udp_ports().empty()) {
        auto p = new evpp::udp::Server();
        udp_server_.reset(p);
        rc = udp_server_->Init(op->udp_ports());
        if (!rc) {
            LOG(ERROR) << "UDP server start failed : "
                       << "udp_ports=" << op->udp_ports();
            return false;
        }
    }

    return true;
}

void Resource::AfterFork() {
    base_loop()->loop()->AfterFork();
    http_server()->AfterFork();
}

void Resource::RunServers() {
    bool rc = true;

    rc = tcp_server_->Start();
    if (!rc) {
        LOG(FATAL) << "TCPServer start failed\n";
    }

    rc = http_server_->Start();
    if (!rc) {
        LOG(FATAL) << "HTTPServer start failed\n";
    }

    rc = udp_server_->Start();
    if (!rc) {
        LOG(FATAL) << "UDPServer start failed\n";
    }

    rc = base_loop()->Start(true);
    if (!rc) {
        LOG(FATAL) << "Base EventLoop start failed\n";
    }
}

void Resource::StopServers() {
    // Stop listen and do not receive new request
    if (udp_server_) {
        udp_server_->Pause();
    }

    if (http_server_) {
        http_server_->Pause();
    }

    if (base_loop_) {
        base_loop_->Stop(false);
    }

    // sleep 1 seconds to wait the last requests done.
    sleep(1);

    // Stop servers
    if (udp_server_) {
        udp_server_->Stop(false);
    }

    if (http_server_) {
        http_server_->Stop();
    }

    if (base_loop_) {
        base_loop_->Stop(false);
    }

    if (tcp_server_) {
        tcp_server_->Stop();
    }

    for (;; sleep(1)) {
        google::FlushLogFiles(0);
        if (base_loop_ && !base_loop_->IsStopped()) {
            LOG(INFO) << "Base EventLoop is stopping now, please wait ...";
            continue;
        }

        if (tcp_server_ && !tcp_server_->IsStopped()) {
            LOG(INFO) << "tcp_server is stopping now, please wait ...";
            continue;
        }

        if (udp_server_ && !udp_server_->IsStopped()) {
            LOG(INFO) << "udp_server is stopping now, please wait ...";
            continue;
        }

        if (http_server_ && !http_server_->IsStopped()) {
            LOG(INFO) << "http_server is stopping now, please wait ...";
            continue;
        }

        break;
    }

    LOG(WARNING) << "All server is stopped.";
}

}
