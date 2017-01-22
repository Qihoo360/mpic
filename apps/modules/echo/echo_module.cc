
#include <evpp/exp.h>
#include <evpp/http/http_server.h>
#include <evpp/tcp_server.h>
#include <evpp/udp/udp_server.h>

#include "echo_module.h"

#include <atomic>

#include <mpic/exp.h>
#include <mpic/master.h>
#include <mpic/title.h>
#include <mpic/option.h>

namespace nfmpic {

std::atomic<bool> running = true;

void sigterm(int c) {
    std::string title_prefix = mpic::Option::GetExeName() + "(" + mpic::Master::instance().option()->name() + "): worker process is shutting down ...";
    mpic::Title::Set(title_prefix);
    running = false;
}


bool EchoModule::InitInMaster(const mpic::Option* op) {
    return true;
}

bool EchoModule::InitInWorker(const mpic::Option* op) {
    signal(SIGTERM, &sigterm);
    Resource* r = GetResource();
    r->AfterFork();
    r->RunServers();
    return true;
}

int EchoModule::Run() {
    LOG(INFO) << __FUNCTION__ << " running ...";
    while (running) {
        google::FlushLogFiles(0);
        usleep(1);
    }

    assert(!running);
    GetResource()->StopServers();
    LOG(WARNING) << "pid=" << getpid() << " exited.";
    return 0;
}

}

EXPORT_MPIC_MODULE(nfmpic::EchoModule, nfmpic::Resource);
