#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include "master.h"
#include "module.h"
#include "title.h"
#include "internal/inner_pre.h"
#include "internal/file_util.h"
#include "internal/dynlib.h"

namespace mpic {

Master Master::instance_;

Master::Master() : dlmodule_(NULL), can_reload_(true) {
}

Master::~Master() {
    UnloadModule();
}

bool Master::Init(int argc, char** argv, std::shared_ptr<Option> op) {
    option_ = op;
    Title::Init(argc, argv);
    if (!option_->Init(argc, argv)) {
        return false;
    }
    return true;
}

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

bool Master::LoadModule() {
    std::string module_path = option_->module_file();
    if (!FileUtil::IsFileExist(module_path) ||
            !FileUtil::IsReadable(module_path)) {
        LOG(ERROR) << "Can't find or read module file " << module_path << std::endl;
        return false;
    }

    dlmodule_.reset(new DynLib(module_path));
    if (!dlmodule_->Load()) {
        LOG(ERROR) << "dlopen(" << module_path << ", ...): " << dlmodule_->GetLastError();
        return false;
    }

    if (!InitResource()) {
        LOG(ERROR) << "InitResource failed.";
        return false;
    }

    if (!InitModule()) {
        LOG(ERROR) << "InitResource failed.";
        return false;
    }

    return true;
}

bool Master::InitResource() {
    typedef Resource* (*ResourceLoader)();

    // Create a Resource instance
    const char* name = "MPIC_NewResource";
    ResourceLoader rloader = (ResourceLoader)dlmodule_->GetSymbol(name);
    if (!rloader) {
        LOG(ERROR) << "dlsym(" << name << "): " << dlmodule_->GetLastError();
        return false;
    }

    try {
        Resource* r = (Resource*)rloader();
        resource_.reset(r);
        if (!r->Init(option_.get())) {
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

bool Master::InitModule() {
    typedef Module* (*ModuleLoader)();

    // Create a Module instance
    const char* name = "MPIC_NewModule";
    ModuleLoader mloader = (ModuleLoader)dlmodule_->GetSymbol(name);

    if (!mloader) {
        LOG(ERROR) << "dlsym(" << name << "): " << dlmodule_->GetLastError();
        return false;
    }
    try {
        Module* m = (Module*)mloader();
        module_.reset(m);
        m->SetResource(resource_.get());
        if (!m->InitInMaster(option_.get())) {
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

void Master::UnloadModule() {
    if (module_) {
        module_->Uninit();
        module_.reset();
    }
    resource_.reset();

    if (dlmodule_) {
        dlmodule_->Unload();
        dlmodule_.reset();
    }
}

const char* Master::GetExeName() const {
    return Option::GetExeName().data();
}

}
