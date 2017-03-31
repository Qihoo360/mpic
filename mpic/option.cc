#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <sstream>
#include <iostream>

#include "platform_config.h"
#include "option.h"
#include "internal/file_util.h"

#include <boost/program_options.hpp>

namespace mpic {

Option::Option()
    : initialized_(false)
    , kill_(false)
    , reload_(false)
    , status_(false)
    , newbin_(false)
    , foreground_(false)
    , debug_(false)
    , worker_processes_(1) {
    vm_.reset(new po::variables_map);
    cmdline_options_.reset(new po::options_description);
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

    std::string tmp_log_dir;
    std::string tmp_pid;
    std::string tmp_cfg;
    std::string tmp_mod;

    std::string name_opt_desc = Option::GetExeName() + " config file name [REQUIRED]";
    po::options_description generic("Generic options");
    generic.add_options()
        ("name,n", po::value<std::string>(&name_), name_opt_desc.data())
        ("help,h", "show this message")
        ("worker_processes,p", po::value<int>(&worker_processes_), "the number of worker processes")
        ("kill,k", "kill the running process")
        ("reload,r", "reload the running process")
        ("status,s", "check mpic process status")
        ("newbin,R", "run new binary")
        ("foreground,f", "run in foreground")
        ("debug,d", "run worker process only")
        ("log_dir",
         po::value<std::string>(&tmp_log_dir)->default_value("/tmp"),
         "log file directory")
        ("pid",
         po::value<std::string>(&tmp_pid)->default_value("/tmp/mpic.pid"),
         "pid file name with full path")
        ("cfg",
         po::value<std::string>(&tmp_cfg)->default_value("/tmp/mpic.conf"),
         "config file name with full path")
        ("mod",
         po::value<std::string>(&tmp_mod)->default_value("/tmp/echo.so"),
         "the application module file name with full path");

    cmdline_options_->add(generic);

    try {
        po::store(po::parse_command_line(argc, argv, *cmdline_options_), *vm_);
    } catch (const std::exception& e) {
        std::cerr << __FILE__ << ":" << __LINE__ << " Error parsing args: " << e.what() << std::endl;
        return false;
    }
    po::notify(*vm_);

    if (vm_->count("help")) {
        std::cout << "Usage: " << argv[0];
        std::cout << " -n <name> [-k|-r|-s|-f|-d|-R] [options...]";
        std::cout << std::endl << *cmdline_options_ << std::endl;
        return false;
    }
    if (this->name_.empty()) {
        this->name_ = FileUtil::GetFileNameWithoutExt(cfg_file_);
    }

    if (vm_->count("kill")) {
        kill_ = true;
    }
    if (vm_->count("reload")) {
        reload_ = true;
    }
    if (vm_->count("status")) {
        status_ = true;
    }
    if (vm_->count("newbin")) {
        newbin_ = true;
    }
    if (vm_->count("foreground")) {
        foreground_ = true;
    }
    if (vm_->count("debug")) {
        debug_ = true;
    }

    if (tmp_log_dir[0] != '/') {
        std::string d = FileUtil::RealPath(tmp_log_dir);
        if (d.empty()) {
            return false;
        }
        tmp_log_dir = d;
    }
    log_dir_ = tmp_log_dir + "/";
    if (FLAGS_log_dir.empty()) {
        FLAGS_log_dir = log_dir_; // TODO check it is right.
    }

    if (tmp_cfg[0] != '/') {
        std::string d = FileUtil::RealPath(tmp_cfg);
        if (d.empty()) {
            return false;
        }
        tmp_cfg = d;
    }
    cfg_file_ = tmp_cfg;

    if (tmp_pid[0] != '/') {
        std::string d = FileUtil::RealPath(tmp_pid);
        if (d.empty()) {
            return false;
        }
        tmp_pid = d;
    }
    pid_file_ = tmp_pid;

    if (tmp_mod[0] != '/') {
        std::string d = FileUtil::RealPath(tmp_mod);
        if (d.empty()) {
            return false;
        }
        tmp_mod = d;
    }
    module_file_ = tmp_mod;


    if (!FileUtil::IsDir(log_dir_)) {
        umask(002);
#ifdef _WIN32
        mkdir(log_dir_.c_str());
#else
        mkdir(log_dir_.c_str(), 0775);
#endif
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

void Option::AddOption(const boost::program_options::options_description& op) {
    cmdline_options_->add(op);
}

const std::string& Option::GetExeName() {
    static std::string name;
    if (name.empty()) {
        char buf[1024] = { 0 };
#ifdef _WIN32
        //::GetModuleFileNameA(NULL, buf, sizeof(buf));
        name = "mpic"; // TODO FIX
#else
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
#endif
    }

    return name;
}

}

