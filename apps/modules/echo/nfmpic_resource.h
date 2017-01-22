#pragma once

#include <memory>

#include <mpic/module.h>

namespace evpp {
class EventLoop;
class TCPServer;

namespace udp { class Server; }
namespace http { class Server; }

}

// Network framework of multi-process in C++
namespace nfmpic {
class Resource : public mpic::Resource {
public:
    Resource();

    // The resource object is created in master process
    // and it will not be changed when this mpic framework reloads
    virtual bool Init(const mpic::Option* op);
    virtual ~Resource() {}

    void AfterFork();
    void RunServers();
    void StopServers();

    std::shared_ptr<evpp::EventLoopThread> base_loop() const {
        return base_loop_;
    }

    std::shared_ptr<evpp::TCPServer> tcp_server() const {
        return tcp_server_;
    }

    std::shared_ptr<evpp::udp::Server> udp_server() const {
        return udp_server_;
    }

    std::shared_ptr<evpp::http::Server> http_server() const {
        return http_server_;
    }

private:
    std::shared_ptr<evpp::EventLoopThread> base_loop_;
    std::shared_ptr<evpp::TCPServer> tcp_server_;
    std::shared_ptr<evpp::udp::Server> udp_server_;
    std::shared_ptr<evpp::http::Server> http_server_;
};

}