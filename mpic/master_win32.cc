#include "./master.h"
#include "./module.h"
#include "./option.h"


namespace mpic {

#ifdef _WIN32
int Master::RunMaster(const Option& op) {
    LOG(INFO) << "Entering " << __func__ << " ...";

    if (!LoadModule()) {
        LOG(ERROR) << "InitModule failed";
        return 1;
    }

    module_->InitInWorker(option_.get());
    module_->Run();

    module_->Uninit();
    module_.reset();

    resource_.reset();
    return 0;
}

int Master::Run() {
    FLAGS_alsologtostderr = true;
    return RunForeground(*option_);
}
#endif

}
