#pragma once
#include "mpic/exp.h"

#include "nfmpic_resource.h"

namespace nfmpic {
class EchoModule : public mpic::Module {
public:
    virtual bool InitInMaster(const mpic::Option* op);
    virtual bool InitInWorker(const mpic::Option* op);
    virtual int Run();

    Resource* GetResource() const {
        return static_cast<nfmpic::Resource*>(resource_);
    }

    void HTTPRequestHandler(evpp::EventLoop* loop,
                            const evpp::http::ContextPtr& ctx,
                            const evpp::http::HTTPSendResponseCallback& cb);

    void UDPRequestHandler(evpp::EventLoop* loop,
                           evpp::udp::MessagePtr& msg);

    void OnTCPMessage(const evpp::TCPConnPtr& conn,
                   evpp::Buffer* msg);

    void OnTCPConnection(const evpp::TCPConnPtr& conn);
};
}
