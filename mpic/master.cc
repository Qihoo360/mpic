#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include <glog/logging.h>

#include "./master.h"
#include "./module.h"
#include "./title.h"
#include "./internal/file_util.h"
#include "internal/dynlib.h"

namespace mpic {

Master Master::instance_;

int Master::RunMainRoutine(const Option& op) {
    FLAGS_stderrthreshold = 0;
    FLAGS_log_dir = op.log_dir();

    if (op.foreground()) {
        google::InitGoogleLogging(Option::GetExeName().data());
    } else {
        google::InitGoogleLogging("master");
    }
    return RunMaster(op);
}

int Master::RunForeground(const Option& op) {
    return RunMainRoutine(op);
}

Master::Master() : dlmodule_(NULL) {
}

Master::~Master() {
}

bool Master::Init(int argc, char** argv, std::shared_ptr<Option> op) {
    option_ = op;
    Title::Init(argc, argv);
    if (!option_->Init(argc, argv)) {
        return false;
    }
    return true;
}

bool Master::InitModule() {
    std::string module_path = option_->module_file();
    if (!FileUtil::IsFileExist(module_path) ||
            !FileUtil::IsReadable(module_path)) {
        std::cerr << "Can't find or read module file " << module_path << std::endl;
        return false;
    }

    dlmodule_.reset(new DynLib(module_path));
    if (!dlmodule_->Load()) {
        LOG(ERROR) << "dlopen(" << module_path << ", ...): " << dlmodule_->GetLastError();
        return false;
    }

    typedef Resource* (*ResourceLoader)();
    typedef Module* (*ModuleLoader)();

    // new Resource
    const char* name = "MPIC_NewResource";
    ResourceLoader rloader = (ResourceLoader)dlmodule_->GetSymbol(name);
    if (!rloader) {
        LOG(ERROR) << "dlsym(" << name << "): " << dlmodule_->GetLastError();
        return false;
    }

    try {
        Resource* r = rloader();
        resource_.reset(r);
        if (!r->Init(option_)) {
            LOG(ERROR) << "Resource init failed ";
            return false;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Load module with exception: " << e.what();
        return false;
    } catch (...) {
        LOG(ERROR) << "Load module failed with unknown error";
        return false;
    }


    // new Module
    name = "MPIC_NewModule";
    ModuleLoader mloader = (ModuleLoader)dlmodule_->GetSymbol(name);

    if (!mloader) {
        LOG(ERROR) << "dlsym(" << name << "): " << dlmodule_->GetLastError();
        return false;
    }
    try {
        Module* m = mloader();
        module_.reset(m);
        m->SetResource(resource_.get());
        if (!m->Init(option_)) {
            LOG(ERROR) << "Resource init failed ";
            return false;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Load module with exception: " << e.what();
        return false;
    } catch (...) {
        LOG(ERROR) << "Load module failed with unknown error";
        return false;
    }

    return true;
}

const char* Master::GetExeName() const {
    return Option::GetExeName().data();
}

}
