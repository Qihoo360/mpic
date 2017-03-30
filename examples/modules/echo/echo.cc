#include "echo.h"

#include "mpic/option.h"


namespace echo {

bool EchoModule::InitInMaster(const mpic::Option* op) {
    option_ = op;
    return true;
}

int EchoModule::Run() {
    LOG(INFO) << __FUNCTION__ << " running ...";
    for (;;) {
        LOG(INFO) << __FUNCTION__ << " EchoModule pid=" << getpid() << " running ...";
        google::FlushLogFiles(0);
        sleep(1);
    }

    return 0;
}

}

EXPORT_MPIC_MODULE(echo::EchoModule, echo::EchoResource);
