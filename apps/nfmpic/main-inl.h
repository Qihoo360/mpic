
#ifdef _WIN32
//#pragma comment(lib, "glog.lib")
#pragma comment(lib, "mpic_static.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifndef _WIN32
#include <signal.h>
#endif

namespace {
    struct OnStartup {
        OnStartup() {
#ifdef _WIN32
            // Initialize net work.
            WSADATA wsaData;
            // Initialize Winsock 2.2
            int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (err) {
                std::cout << "WSAStartup() failed with error: %d" << err;
            }
#else
            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
                LOG(ERROR) << "SIGPIPE set failed.";
                exit(-1);
            }
            LOG(INFO) << "ignore SIGPIPE";
#endif
        }
        ~OnStartup() {}
    } __s_onstartup;
}