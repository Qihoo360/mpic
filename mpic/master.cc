#include <sys/wait.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include <libdaemon/daemon.h>
#include <glog/logging.h>

#include "./master.h"
#include "./title.h"

namespace mpic {

Master Master::instance_;

static const char* PidFileName() {
    return Master::instance().option()->pid_file().c_str();
}


static void sigchld(int) {}

void Master::KillAllChildren() {
    exiting_processes_ = running_processes_;
    ProcessMap::iterator it(running_processes_.begin());
    ProcessMap::iterator ite(running_processes_.end());
    for (; it != ite; ++it) {
        LOG(INFO) << "killing child(" << it->first << ")";
        kill(it->first, SIGTERM);
    }
    running_processes_.clear();
}

pid_t Master::SpawnChildWorker(const mpic::Option& op, sigset_t* sigset) {
    google::FlushLogFiles(0);
    pid_t pid = fork();
    if (pid < 0) {
        PLOG(FATAL) << "fork() failed!!";
    } else if (pid == 0) {
        // child
        LOG(INFO) << "in child process, child (" << getpid() << ") started";
        sigprocmask(SIG_UNBLOCK, sigset, NULL);
        if (!op.foreground()) {
            std::string title_prefix = mpic::Option::GetExeName() + "(" + op.name() + "): worker process";
            mpic::Title::Set(title_prefix);
        }

        running_processes_.clear(); // Now we don't need to use it in children worker process
        exiting_processes_.clear();

        google::ShutdownGoogleLogging();
        google::InitGoogleLogging(mpic::Option::GetExeName().data());
        exit(Master::instance().worker_main_routine()());
    }

    // parent
    assert(pid > 0);
    LOG(INFO) << "in master process, child(" << pid << ") started";

    Process p;
    p.pid = pid;
    running_processes_[pid] = p;
    return pid;
}

void Master::SpawnChildWorkers(const mpic::Option& op, sigset_t* sigset) {
    for (int i = 0; i < op.worker_processes(); ++i) {
        SpawnChildWorker(op, sigset);
    }
}

int Master::RunMaster(const mpic::Option& op) {
    LOG(INFO) << "Entering " << __func__ << " ...";

    std::string origin_title = op.original_cmdline();
    std::string title_prefix = mpic::Option::GetExeName() + "(" + op.name() + "): master process";
    mpic::Title::Set(title_prefix + origin_title);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGINT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    signal(SIGCHLD, sigchld);

    SpawnChildWorkers(op, &sigset);

    bool exiting = false;

    while (true) {
        siginfo_t sig;
        int result = sigwaitinfo(&sigset, &sig);
        if (result < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            PLOG(ERROR) << "sigwaitinfo: ";
            return 1;
        }

        if (sig.si_signo == SIGCHLD) {
            LOG(INFO) << "child signal received";
            for (;;) {
                int status = 0;
                pid_t pid = ::waitpid(-1, &status, WNOHANG);
                if (pid < 0) {
                    PLOG(ERROR) << "waitpid()";
                    break;
                } else if (pid == 0) {
                    PLOG(ERROR) << "waitpid() return 0";
                    break;
                }

                if (WIFEXITED(status)) {
                    LOG(INFO) << "child(" << pid << ") exited with " << WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    LOG(INFO) << "child(" << pid << ") killed by signal " << WTERMSIG(status);
                } else if (WCOREDUMP(status)) {
                    LOG(INFO) << "child(" << pid << ") core dumped";
                } else {
                    LOG(INFO) << "child(" << pid << ") XXX";
                    continue;
                }

                if (exiting_processes_.find(pid) != exiting_processes_.end()) {
                    exiting_processes_.erase(pid);
                    LOG(INFO) << "erased pid(" << pid << ") from s_exiting_processes. now s_exiting_processes num=" << exiting_processes_.size();
                }

                if (running_processes_.find(pid) != running_processes_.end()) {
                    running_processes_.erase(pid);
                    SpawnChildWorker(op, &sigset);
                }
            }

            if (exiting) {
                break;
            }
        } else if (sig.si_signo == SIGHUP) {
            LOG(INFO) << "SIGHUP reload signal recved. signo=" << sig.si_signo;
            if (!exiting_processes_.empty()) {
                LOG(WARNING) << "The master has been already reloading, we ignore this SIGHUP reload signal";
                continue;
            }
            // TODO FIX bug : this code must be spawn children first and then kill old children.
            KillAllChildren();
            SpawnChildWorkers(op, &sigset);
        } else if (sig.si_signo == SIGTERM) {
            LOG(INFO) << "term signal recved. signo=" << sig.si_signo;
            KillAllChildren();
            exiting = true;
        } else {
            LOG(ERROR) << "signal recved. signo=" << sig.si_signo << " , nO process handler";
        }
    }
    return 0;
}

int Master::RunMainRoutine(const mpic::Option& op) {
    FLAGS_stderrthreshold = 0;
    FLAGS_log_dir = op.log_dir();

    if (op.foreground()) {
        google::InitGoogleLogging(mpic::Option::GetExeName().data());
        return Master::instance().worker_main_routine()();
    } else {
        google::InitGoogleLogging("master");
        return RunMaster(op);
    }
}

int Master::RunForeground(const mpic::Option& op) {
    return RunMainRoutine(op);
}

int Master::RunAsDaemon(const mpic::Option& op) {
    daemon_pid_file_proc = PidFileName;
    daemon_log_ident = op.name().c_str();

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        daemon_log(LOG_ERR, "%s daemon is already running, pid=%d", GetExeName(), pid);
        return 1;
    }
    if (daemon_retval_init() != 0) {
        daemon_log(LOG_ERR, "initialize daemon failed");
        return 1;
    }

    pid = daemon_fork();
    if (pid < 0) {
        daemon_log(LOG_ERR, "daemonize failed: %s",
                   strerror(errno));
        daemon_retval_done();
        return 1;
    } else if (pid > 0) {
        // in parent
        int retval = 0;
        if ((retval = daemon_retval_wait(10)) < 0) {
            daemon_log(LOG_ERR, "can't retrive daemon retval: %s",
                       strerror(errno));
            return 1;
        }
        switch (retval) {
        case 0:
            daemon_log(LOG_ERR, "%s daemon started, pid=%d", GetExeName(), pid);
            break;
        default:
            daemon_log(LOG_ERR, "%s daemon_retval_wait ERROR, retval=%d", GetExeName(), retval);
            break;
        }
        return retval;
    } else {
        // in daemon
        daemon_log(LOG_INFO, "in daemon pid %d", pid);
        int retval = 0;
        if (daemon_close_all(-1) != 0) {
            daemon_log(LOG_ERR, "can't close all fds: %s",
                       strerror(errno));
            retval = 1;
            goto finish;
        }
        if (daemon_pid_file_create() != 0) {
            daemon_log(LOG_ERR, "can't write pid file %s: %s",
                       op.pid_file().c_str(), strerror(errno));
            retval = 2;
            goto finish;
        }
        daemon_retval_send(0);
        daemon_log(LOG_INFO, "%s daemon start ok.", GetExeName());
        umask(022);
        RunMainRoutine(op);
    finish:
        if (retval != 0) {
            daemon_retval_send(retval);
        }
        daemon_log(LOG_INFO, "%s daemon exiting...", GetExeName());
        daemon_pid_file_remove();
        return retval;
    }
}

int Master::KillDaemon(const mpic::Option& op) {
    daemon_pid_file_proc = PidFileName;
    pid_t pid = daemon_pid_file_is_running();
    if (pid > 0) {
        fprintf(stdout, "%s(%s) is running, pid=%d\n",
                GetExeName(), op.name().c_str(), pid);
        if (daemon_pid_file_kill_wait(SIGTERM, 5)) {
            if (errno == ETIME) {
                fprintf(stderr, "%s is still running, "
                        "maybe waiting for flush logs\n.", GetExeName());
            } else {
                fprintf(stderr, "can't kill: %s\n", strerror(errno));
            }
            return 1;
        }
        fprintf(stdout, "killed\n");
        return 0;
    } else {
        fprintf(stderr, "%s(%s) is not running\n",
                GetExeName(), op.name().c_str());
        return 1;
    }
}

int Master::ReloadDaemon(const mpic::Option& op) {
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid > 0) {
        fprintf(stdout, "%s(%s) is running, pid=%d\n",
                GetExeName(), op.name().c_str(), pid);
        if (daemon_pid_file_kill(SIGHUP)) {
            fprintf(stderr, "can't reload: %s\n", strerror(errno));
            return 1;
        }
        fprintf(stdout, "reloaded\n");
        return 0;
    } else {
        fprintf(stderr, "%s(%s) is not running\n",
                GetExeName(), op.name().c_str());
        return 1;
    }
}

int Master::CheckStatus(const mpic::Option& op) {
    daemon_pid_file_proc = PidFileName;
    pid_t pid = daemon_pid_file_is_running();
    if (pid > 0) {
        fprintf(stdout, "%s(%s) is running, pid=%d\n",
                GetExeName(), op.name().c_str(), pid);
        return 0;
    } else {
        fprintf(stderr, "%s(%s) is not running\n",
                GetExeName(), op.name().c_str());
        return 1;
    }
}

Master::Master() {
}

Master::~Master() {
}

int Master::Run(WorkerMainRoutine worker_main) {

    worker_main_routine_ = worker_main;

    if (option_->kill()) {
        return KillDaemon(*option_);
    }

    if (option_->reload()) {
        return ReloadDaemon(*option_);
    }

    if (option_->status()) {
        return CheckStatus(*option_);
    }

    if (option_->foreground()) {
        FLAGS_alsologtostderr = true;
        return RunForeground(*option_);
    } else {
        return RunAsDaemon(*option_);
    }
}

bool Master::Init(int argc, char** argv, std::shared_ptr<Option> op) {
    option_ = op;
    mpic::Title::Init(argc, argv);
    if (!option_->Init(argc, argv)) {
        return false;
    }
    return true;
}


}
