#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "mpic/daemonizer.h"

int main(int argc, char* argv[]) {
    printf("pid=%d starting ...\n", getpid());
    pid_t start_pid = getpid();
    bool is_daemon = false;
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        is_daemon = true;
        mpic::Daemonizer daemon;
        daemon.Init();
    }

    pid_t pid = getpid();
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


