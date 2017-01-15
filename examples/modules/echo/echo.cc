#include "echo.h"

#include "mpic/option.h"

namespace echo {

    bool EchoModule::Init(const std::shared_ptr<mpic::Option>& op) {
        option_ = op;
        return true;
    }

    int EchoModule::Run() {
        LOG(INFO) << __FUNCTION__ << " running ...";
        for (;;) {
            LOG(INFO) << __FUNCTION__ << " pid=" << getpid() << " running ...";
            google::FlushLogFiles(0);
            sleep(1);
        }

        return 0;
    }

}

MPIC_CREATE_MODULE(echo::EchoModule);
MPIC_CREATE_RESOURCE(echo::EchoResource);