


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
                LOG(ERROR) << "SIGPIPE set failed.";
                exit(-1);
            }
            LOG(INFO) << "ignore SIGPIPE";
#endif
        }
        ~OnStartup() {}
    } __s_onstartup;
}