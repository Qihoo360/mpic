#pragma once

#include <string>

#include "platform_config.h"
#include "mpic_export.h"
#include "logging.h"


namespace boost {
namespace program_options {
class variables_map;
class options_description;
}
}

namespace mpic {

namespace po = boost::program_options;

class MPIC_EXPORT Option {
public:
    Option();
    virtual ~Option();
    virtual bool Init(int argc, char** argv);

public:
    void AddOption(const po::options_description& op);

    const po::variables_map* vm() const {
        return vm_.get();
    }

    const po::options_description* cmdline_options() const {
        return cmdline_options_.get();
    }

public:
    bool kill() const {
        return kill_;
    }
    bool reload() const {
        return reload_;
    }
    bool status() const {
        return status_;
    }
    bool newbin() const {
        return newbin_;
    }
    bool debug()  const {
        return debug_;
    }
    bool foreground() const {
        return foreground_;
    }
    const std::string& name() const {
        return name_;
    }
    const std::string& log_dir() const {
        return log_dir_;
    }
    const std::string& pid_file() const {
        return pid_file_;
    }
    const std::string& cfg_file() const {
        return cfg_file_;
    }
    const std::string& module_file() const {
        return module_file_;
    }
    const std::string& original_cmdline() const {
        return original_cmdline_;
    }
    int worker_processes() const {
        return worker_processes_;
    }

public:

    static const std::string& GetExeName();

private:
    std::shared_ptr<po::variables_map> vm_;
    std::shared_ptr<po::options_description> cmdline_options_;

    std::string original_cmdline_;
    bool initialized_;

    // Options only init with command line.
    std::string name_;
    bool kill_;
    bool reload_;
    bool status_;
    bool newbin_;
    bool foreground_;
    bool debug_;
    std::string log_dir_;
    std::string pid_file_;
    std::string cfg_file_;
    std::string module_file_; // the module so file path
    int worker_processes_; // The number of worker processes
};
}
