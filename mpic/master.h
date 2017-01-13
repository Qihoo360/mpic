#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <functional>
#include <memory>
#include <map>

#include "option.h"

namespace mpic {

// 1 - Master process
// N - Worker processes

// @see test/master_worker_test.cpp for the detail usage
class Master {
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
     * @param worker_main - the child worker process main routine
     * @return 0 if OK, others failed
     */
    int Run(WorkerMainRoutine worker_main);

public:
    const std::shared_ptr<Option>& option() const {
        return option_;
    }

    WorkerMainRoutine worker_main_routine() {
        return worker_main_routine_;
    }

    static Master& instance() {
        return instance_;
    }
private:
    const char* GetExeName() {
        return mpic::Option::GetExeName().data();
    }

    pid_t SpawnChildWorker(const mpic::Option& option, sigset_t* sigset);

    void SpawnChildWorkers(const mpic::Option& option, sigset_t* sigset);

    int RunMaster(const mpic::Option& option);

    int RunMainRoutine(const mpic::Option& option);

    int RunForeground(const mpic::Option& option);

    int RunAsDaemon(const mpic::Option& option);

    // ops interface : KillDaemon ReloadDaemon CheckStatus
    int KillDaemon(const mpic::Option& option);
    int ReloadDaemon(const mpic::Option& option);
    int CheckStatus(const mpic::Option& option);

private:
    std::shared_ptr<Option> option_;
    WorkerMainRoutine worker_main_routine_;

    struct Process {
        pid_t pid;
        // Add other data field here.
        // PIPE fd
    };
    typedef std::map<pid_t, Process> ProcessMap;
    ProcessMap running_processes_; // Current running worker processes
    ProcessMap exiting_processes_; // worker processes which are exiting

private:
    void KillAllChildren(const ProcessMap& m);
private:
    Master();
    Master(const Master& rhs) {}

    static Master instance_;
};
}

