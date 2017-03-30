
#include <evpp/exp.h>
#include <evpp/http/http_server.h>
#include <evpp/tcp_server.h>
#include <evpp/tcp_conn.h>
#include <evpp/udp/udp_server.h>

#include "echo_module.h"

#include <atomic>

#include <mpic/exp.h>
#include <mpic/master.h>
#include <mpic/title.h>
#include <mpic/option.h>

#ifdef H_OS_WINDOWS
#pragma comment(lib,"libmpic.lib")
#endif

namespace nfmpic {

std::atomic<bool> running(true);

void sigterm(int c) {
    std::string title_prefix = mpic::Option::GetExeName() + "(" + mpic::Master::instance().option()->name() + "): worker process is shutting down ...";
    mpic::Title::Set(title_prefix);
    running.store(false);
}


bool EchoModule::InitInMaster(const mpic::Option* op) {
    return true;
}

bool EchoModule::InitInWorker(const mpic::Option* op) {
    signal(SIGTERM, &sigterm);
    Resource* r = GetResource();

    r->AfterFork();
    r->http_server()->RegisterHandler("/echo", std::bind(&EchoModule::HTTPRequestHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    r->udp_server()->SetMessageHandler(std::bind(&EchoModule::UDPRequestHandler, this, std::placeholders::_1, std::placeholders::_2));
    r->tcp_server()->SetConnectionCallback(std::bind(&EchoModule::OnTCPConnection, this, std::placeholders::_1));
    r->tcp_server()->SetMessageCallback(std::bind(&EchoModule::OnTCPMessage, this, std::placeholders::_1, std::placeholders::_2));

    r->RunServers();
    return true;
}

int EchoModule::Run() {
    LOG(INFO) << __FUNCTION__ << " running ...";
    google::FlushLogFiles(0);

    while (running.load()) {
        usleep(300*1000);
    }

    assert(!running.load());

    LOG(INFO) << __FUNCTION__ << " stopping ...";
    google::FlushLogFiles(0);
    GetResource()->StopServers();
    LOG(WARNING) << "pid=" << getpid() << " exited.";
    return 0;
}


void EchoModule::HTTPRequestHandler(evpp::EventLoop* loop, const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& cb) {
    std::stringstream oss;
    oss << "func=" << __FUNCTION__ << " OK"
        << " ip=" << ctx->remote_ip() << "\n"
        << " uri=" << ctx->uri() << "\n"
        << " body=" << ctx->body().ToString() << "\n";
    cb(oss.str());
}


void EchoModule::UDPRequestHandler(evpp::EventLoop* loop, evpp::udp::MessagePtr& msg) {
    std::stringstream oss;
    oss << "func=" << __FUNCTION__ << " OK"
        << " body=" << std::string(msg->data(), msg->size()) << "\n";
    evpp::udp::SendMessage(msg);
}


void EchoModule::OnTCPMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg) {
    std::string s = msg->NextAllString();
    LOG_INFO << "Received a message [" << s << "]";
    conn->Send(s);

    if (s == "quit" || s == "exit") {
        conn->Close();
    }
}


void EchoModule::OnTCPConnection(const evpp::TCPConnPtr& conn) {
    if (conn->IsConnected()) {
        LOG_INFO << "Accept a new connection from " << conn->remote_addr();
    } else {
        LOG_INFO << "Disconnected from " << conn->remote_addr();
    }
}

}

EXPORT_MPIC_MODULE(nfmpic::EchoModule, nfmpic::Resource);
