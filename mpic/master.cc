#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

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

static void sigchld(int) {}


static pid_t SpawnChildWorker(sigset_t* sigset) {
    pid_t pid = fork();
    if (pid < 0) {
        abort();
    } else if (pid == 0) {
        // child
        std::string title = std::string(g_exe_name) + "(mpic): worker process";
        Title::Set(title);
        sigprocmask(SIG_UNBLOCK, sigset, NULL);
        exit(Master::instance().worker_main_routine()());
    } else if (pid > 0) {
        // parent
    }
    return pid;
}

static int RunMaster() {
    std::string title = std::string(g_exe_name) + "(mpic): monitor process";
    Title::Set(title);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    signal(SIGCHLD, sigchld);

    pid_t child = SpawnChildWorker(&sigset);
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
            int status = 0;
            pid_t pid = ::waitpid(-1, &status, WNOHANG);
            if (pid < 0) {
                continue;
            } else if (pid == 0) {
                continue;
            }

            if (WIFEXITED(status)) {
                //LOG(INFO) << "child(" << pid << ") exited with " << WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                //LOG(INFO) << "child(" << pid << ") killed by signal " << WTERMSIG(status);
            } else if (WCOREDUMP(status)) {
                //LOG(INFO) << "child(" << pid << ") core dumped";
            } else {
                //LOG(INFO) << "child(" << pid << ") XXX status=" << status;
                continue;
            }
            if (exiting) {
                break;
            }
            if (pid == child) {
                //sleep(1); // Why need? comments it.
                child = SpawnChildWorker(&sigset);
            }
        } else if (sig.si_signo == SIGHUP) {
            //LOG(INFO) << "SIGHUP reload signal recved";
            kill(child, SIGTERM);
            // then this Master will recv a SIGCHLD signal and restart the worker again
        } else {
            //LOG(INFO) << "term signal recved";
            kill(child, SIGTERM);
            exiting = true;
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

Master::Master() {
}

Master::~Master() {
}

int Master::Run(WorkerMainRoutine worker_main) {
    worker_main_routine_ = worker_main;
    return detail::RunAsDaemon();
}

bool Master::Init(int argc, char** argv) {
    Title::Init(argc, argv);
    return true;
}

}
