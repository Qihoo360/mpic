#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <sstream>
#include <iostream>

#include <gflags/gflags.h>

#include "option.h"
#include "file_util.h"

DEFINE_string(config_file, "/tmp/mpic.conf", "The config file path for the submodule");
DEFINE_string(pid_file, "/tmp/mpic.pid", "The pid file path of the process");
DEFINE_string(name, "", "The name of the submodule");
DEFINE_string(module_file, "/tmp/mpic.so", "The file path of the submodule");
DEFINE_int32(worker_processes, 1, "The count of worker processes");
DEFINE_bool(kill, false, "Kill the process");
DEFINE_bool(reload, false, "Reload the process");
DEFINE_bool(foreground, false, "Run the process in foreground mode, default is in daemon mode");
DEFINE_bool(debug, false, "Run the process in debug mode");
DEFINE_bool(status, false, "Show the status of the process");
DECLARE_string(log_dir);

namespace mpic {

//static const char* kCfgExt = ".conf";
//static const char* kPidExt = ".pid";
//static const char* kDefaultCfgDir = "/var/mpic/etc";
static const char* kDefaultLogDir = "/tmp";
//static const char* kDefaultPidDir = "/var/mpic/run";

Option::Option()
    : initialized_(false)
    , kill_(false)
    , reload_(false)
    , status_(false)
    , newbin_(false)
    , foreground_(false)
    , debug_(false)
    , worker_processes_(1) {
}

Option::~Option() {}

bool Option::Init(int argc, char** argv) {
    if (initialized_) {
        return true;
    }

    std::stringstream title;
    for (int i = 0; i < argc; ++i) {
        title << " " << argv[i];
    }
    original_cmdline_ = title.str();

    if (FLAGS_log_dir.empty()) {
        FLAGS_log_dir = kDefaultLogDir;
    }

    this->kill_ = FLAGS_kill;
    this->reload_ = FLAGS_reload;
    this->foreground_ = FLAGS_foreground;
    this->status_ = FLAGS_status;
    this->worker_processes_ = FLAGS_worker_processes;
    this->cfg_file_ = FLAGS_config_file;
    this->pid_file_ = FLAGS_pid_file;
    this->module_file_ = FLAGS_module_file;
    this->name_ = FLAGS_name;
    this->log_dir_ = FLAGS_log_dir;
    if (this->name_.empty()) {
        this->name_ = FileUtil::GetFileNameWithoutExt(cfg_file_);
    }

    // this->newbin_

    if (!FileUtil::IsDir(log_dir_)) {
        umask(002);
        mkdir(log_dir_.c_str(), 0775);
        umask(022);

        if (!FileUtil::IsDir(log_dir_)) {
            std::cerr << "Can't open log-dir [" << log_dir_ << "]\n";
            return false;
        }
    }

    if (!FileUtil::IsFileExist(cfg_file_) ||
            !FileUtil::IsReadable(cfg_file_)) {
        std::cerr << "Can't find or read config file " << cfg_file_ << std::endl;
        return false;
    }

    if (!FileUtil::IsFileExist(module_file_) ||
            !FileUtil::IsReadable(module_file_)) {
        std::cerr << "Can't find or read module file " << module_file_ << std::endl;
        return false;
    }

    initialized_ = true;

    return true;
}

const std::string& Option::GetExeName() {
    static std::string name;
    if (name.empty()) {
        char buf[1024] = { 0 };
        int count = readlink("/proc/self/exe", buf, 1024);
        if (count < 0 || count >= 1024) {
            printf("Failed to %s\n", __func__);
            return name;
        }
        buf[count] = '\0';

        const char* n = strrchr(buf, '/');
        if (n) {
            name = n + 1;
        } else {
            name = buf;
        }
    }

    return name;
}


std::string Option::RealPath(const std::string& path) {
    assert(!path.empty());
    char buf[4096];
    char* p = realpath(path.c_str(), buf);
    if (p == NULL) {
        std::cout << "convert relative path: " << path
                  << " to real path error: " << strerror(errno);
    }

    return std::string(p);
}
}

