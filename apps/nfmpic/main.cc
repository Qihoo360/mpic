#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "nfmpic_option.h"

#include "mpic/exp.h"
#include "mpic/master.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib,"libmpic.lib")
#pragma comment(lib,"Ws2_32.lib")
#endif

#ifndef H_OS_WINDOWS
#include <signal.h>
#endif

namespace {
    struct OnStartup {
        OnStartup() {
#ifdef H_OS_WINDOWS
            // Initialize net work.
            WSADATA wsaData;
            // Initialize Winsock 2.2
            int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (err) {
                std::cout << "WSAStartup() failed with error: %d" << err;
            }
#else
            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
                LOG_ERROR << "SIGPIPE set failed.";
                exit(-1);
            }
            LOG_INFO << "ignore SIGPIPE";
#endif
        }
        ~OnStartup() {}
    } __s_onstartup;
}


// Run on Windowns:
//      $ cd msvc/bin/Debug
//      $ ./nfmpic.exe -config_file=mpic.conf -module_file=libnfmpic-module-echo.dll
//
// Run on Linux
//
int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    std::shared_ptr<mpic::Option> op(new nfmpic::Option);
    mpic::Master& pm = mpic::Master::instance();
    if (pm.Init(argc, argv, op)) {
        return pm.Run();
    } else {
        LOG(ERROR) << "master init failed.";
        return -1;
    }
}
