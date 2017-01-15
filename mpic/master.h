#pragma once

#include <sys/types.h>
#include <signal.h>
#include <functional>
#include <memory>
#include <map>

#include "platform_config.h"
#include "inner_pre.h"

#include "option.h"

namespace mpic {

class Resource;
class Module;
class DynLib;

// 1 - Master process
// N - Worker processes

// @see test/master_worker_test.cpp for the detail usage
class MPIC_EXPORT Master {
public:
    /** return 0 if OK, others failed */
    typedef std::function< int () > WorkerMainRoutine;

    ~Master();

    /**
     * @param argc
     * @param argv
     * @param op - The application program command line options
     * @return true if initialize OK.
     */
    bool Init(int argc, char** argv, std::shared_ptr<Option> op);

    /**
     * @return 0 if OK, others failed
     */
    int Run();

public:
    const std::shared_ptr<Option>& option() const {
        return option_;
    }

    static Master& instance() {
        return instance_;
    }

private:
    const char* GetExeName();

    int RunMaster(const mpic::Option& option);

    int RunMainRoutine(const mpic::Option& option);

    int RunForeground(const mpic::Option& option);

    int RunAsDaemon(const mpic::Option& option);

    bool InitModule();

#ifndef _WIN32
private:
    //WorkerMainRoutine worker_main_routine_;

    struct Process {
        pid_t pid;
        // Add other data field here.
        // PIPE fd
    };
    typedef std::map<pid_t, Process> ProcessMap;
    ProcessMap running_processes_; // Current running worker processes
    ProcessMap exiting_processes_; // worker processes which are exiting

private:
    pid_t SpawnChildWorker(const mpic::Option& option, sigset_t* sigset);

    void SpawnChildWorkers(const mpic::Option& option, sigset_t* sigset);

    // ops interface : KillDaemon ReloadDaemon CheckStatus
    int KillDaemon(const mpic::Option& option);
    int ReloadDaemon(const mpic::Option& option);
    int CheckStatus(const mpic::Option& option);

    void HandleSIGHUB(const mpic::Option& option, sigset_t* sigset);
    void HandleSIGCHLD(const mpic::Option& option, sigset_t* sigset);
    void KillAllChildren(const ProcessMap& m);
#endif

private:
    std::shared_ptr<Option> option_;
    std::shared_ptr<Resource> resource_;
    std::shared_ptr<Module> module_;
    std::shared_ptr<DynLib> dlmodule_;

private:
    Master();
    Master(const Master& rhs) {}

    static Master instance_;
};
}

