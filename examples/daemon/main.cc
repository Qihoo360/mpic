#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "daemonizer.h"

int main(int argc, char* argv[]) {
    pid_t start_pid = getpid();
    printf("pid=%d starting ...\n", start_pid);
    bool is_daemon = false;
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        is_daemon = true;
        mpic::Daemonizer daemon;
        daemon.Init();
    } else if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        printf("Usage : %s -d\n", argv[0]);
        return 0;
    }

    pid_t pid = getpid();
    printf("pid=%d", pid);
    if (is_daemon) {
        assert(pid != start_pid);
    } else {
        assert(pid == start_pid);
    }

    for (int i = 0; i < 60; i++) {
        printf("i=%d running ...\n", i);
        sleep(1);
    }

    return 0;
}


