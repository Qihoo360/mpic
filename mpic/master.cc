#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <map>

#include <libdaemon/daemon.h>

#include "./master.h"
#include "./title.h"

namespace mpic {

Master Master::instance_;

using namespace std;

static const char* PidFileName() {
    return "/tmp/mpic.pid";
}

static const std::string& GetExeName() {
    static std::string __s_name;
    if (__s_name.empty()) {
        char buf[1024] = {0};
        int count = readlink("/proc/self/exe", buf, 1024);
        if (count < 0 || count >= 1024) {
            printf("Failed to %s\n", __func__);
            return __s_name;
        }
        buf[ count ] = '\0';

        const char* name = strrchr(buf, '/');
        if (name) {
            __s_name = name + 1;
        } else {
            __s_name = buf;
        }
    }

    return __s_name;
}

namespace detail {

static const char* g_exe_name = GetExeName().c_str();

struct Process {
    pid_t pid;
};
typedef std::map<pid_t, Process> ProcessMap;
static ProcessMap s_processes; // Current running worker processes
static ProcessMap s_exiting_processes; // worker processes which are exiting
static void sigchld(int) {}

static void KillAllChildren() {
    s_exiting_processes = s_processes;
    ProcessMap::iterator it(s_processes.begin());
    ProcessMap::iterator ite(s_processes.end());
    for (; it != ite; ++it) {
        //LOG(INFO) << "killing child(" << it->first << ")";
        kill(it->first, SIGTERM);
    }
    s_processes.clear();
}

static pid_t SpawnChildWorker(sigset_t* sigset) {
    pid_t pid = fork();
    if (pid < 0) {
        abort();
    } else if (pid == 0) {
        // child
        std::string title = std::string(g_exe_name) + "(mpic): worker process";
        Title::Set(title);
        sigprocmask(SIG_UNBLOCK, sigset, NULL);
        s_processes.clear(); // Now we don't need to use it in children worker process
        exit(Master::instance().worker_main_routine()());
    } else if (pid > 0) {
        // parent
    }

    // parent
    assert(pid > 0);

    Process p;
    p.pid = pid;
    s_processes[pid] = p;
    return pid;
}

static void SpawnChildWorkers(sigset_t* sigset) {
    for (int i = 0; i < Master::instance().worker_processes(); ++i) {
        SpawnChildWorker(sigset);
    }
}

static int RunMaster() {
    std::string title = std::string(g_exe_name) + "(mpic): master process";
    Title::Set(title);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGINT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    signal(SIGCHLD, sigchld);

    SpawnChildWorkers(&sigset);

    bool exiting = false;

    while (true) {
        siginfo_t sig;
        int result = sigwaitinfo(&sigset, &sig);
        if (result < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            return 1;
        }

        if (sig.si_signo == SIGCHLD) {
            //LOG(INFO) << "child signal recved";
            for (;;) {
                int status = 0;
                pid_t pid = ::waitpid(-1, &status, WNOHANG);
                if (pid < 0) {
                    //PLOG(ERROR) << "waitpid()";
                    break;
                } else if (pid == 0) {
                    //PLOG(ERROR) << "waitpid() return 0";
                    break;
                }

                if (WIFEXITED(status)) {
                    //LOG(INFO) << "child(" << pid << ") exited with " << WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    //LOG(INFO) << "child(" << pid << ") killed by signal " << WTERMSIG(status);
                } else if (WCOREDUMP(status)) {
                    //LOG(INFO) << "child(" << pid << ") core dumped";
                } else {
                    //LOG(INFO) << "child(" << pid << ") XXX";
                    continue;
                }

                if (s_exiting_processes.find(pid) != s_exiting_processes.end()) {
                    s_exiting_processes.erase(pid);
                    //LOG(INFO) << "erased pid(" << pid << ") from s_exiting_processes. now s_exiting_processes num=" << s_exiting_processes.size();
                }

                if (s_processes.find(pid) != s_processes.end()) {
                    s_processes.erase(pid);
                    SpawnChildWorker(&sigset);
                }
            }
            if (exiting) {
                break;
            }
        } else if (sig.si_signo == SIGHUP) {
            //LOG(INFO) << "SIGHUP reload signal recved. signo=" << sig.si_signo;
            if (!s_exiting_processes.empty()) {
                //LOG(WARNING) << "The master has been already reloading, we ignore this SIGHUP reload signal";
                continue;
            }
            KillAllChildren();
            SpawnChildWorkers(&sigset);
        } else if (sig.si_signo == SIGTERM) {
            //LOG(INFO) << "term signal recved. signo=" << sig.si_signo;
            KillAllChildren();
            exiting = true;
        } else {
            //LOG(ERROR) << "signal recved. signo=" << sig.si_signo << " , nO process handler";
        }
    }
    return 0;
}


static int RunAsDaemon() {
    daemon_pid_file_proc = PidFileName;
    daemon_log_ident = "mpic-app";

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        daemon_log(LOG_ERR, "%s daemon is already running, pid=%d", g_exe_name, pid);
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
            daemon_log(LOG_ERR, "%s daemon started, pid=%d", g_exe_name, pid);
            break;
        default:
            daemon_log(LOG_ERR, "%s daemon_retval_wait ERROR, retval=%d", g_exe_name, retval);
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
                       PidFileName(), strerror(errno));
            retval = 2;
            goto finish;
        }
        daemon_retval_send(0);
        daemon_log(LOG_INFO, "%s daemon start ok.", g_exe_name);
        umask(022);
        RunMaster();
finish:
        if (retval != 0) {
            daemon_retval_send(retval);
        }
        daemon_log(LOG_INFO, "%s daemon exiting...", g_exe_name);
        daemon_pid_file_remove();
        return retval;
    }
}

static int ReloadDaemon() {
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid > 0) {
        fprintf(stdout, "%s(%s) is running, pid=%d\n",
                g_exe_name, g_exe_name, pid);
        if (daemon_pid_file_kill(SIGHUP)) {
            fprintf(stderr, "can't reload: %s\n", strerror(errno));
            return 1;
        }
        fprintf(stdout, "reloaded\n");
        return 0;
    } else {
        fprintf(stderr, "%s(%s) is not running\n",
                g_exe_name, g_exe_name);
        return 1;
    }
}


}

Master::Master() : worker_processes_(1) {
}

Master::~Master() {
}

int Master::Run(WorkerMainRoutine worker_main) {
    worker_main_routine_ = worker_main;
    return detail::RunAsDaemon();
}

bool Master::Init(int argc, char** argv) {
    Title::Init(argc, argv);
    if (argc == 2) {
        worker_processes_ = atoi(argv[1]);
    }
    return true;
}

}
