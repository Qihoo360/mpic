#include "./master.h"
#include "./module.h"
#include "./option.h"


namespace mpic {

#ifdef _WIN32
int Master::RunMaster(const Option& op) {
    LOG(INFO) << "Entering " << __func__ << " ...";

    if (!InitModule()) {
        LOG(ERROR) << "InitModule failed";
        return 1;
    }

    if (option_->foreground()) {
        return module_->Run();
    } else {
        return module_->Run();
        //assert(false && "NOT SUPPORT");
    }

    return 0;
}

int Master::Run() {
    FLAGS_alsologtostderr = true;
    return RunForeground(*option_);
}
#endif

}
