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
        assert(false && "NOT SUPPORT");
    }

    return 0;
}
#endif

}
