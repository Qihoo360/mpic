#include "daemonizer.h"

#include <errno.h> 
#include <string.h>
#include <stdlib.h>

#include "libdaemon/daemon.h"

namespace mpic {

Daemonize::Daemonize()
{
}

Daemonize::~Daemonize()
{
  daemon_log(LOG_INFO, "Process daemon gracefully exited ...");
  daemon_retval_send(0);
}

void Daemonize::Init()
{
    /* Prepare for return value passing from the initialization procedure of the daemon process */
    pid_t pid = 0;
    if (daemon_retval_init() < 0) {
        daemon_log(LOG_ERR, "Failed to create pipe.");
    }

    /* Do the fork */
    if ((pid = daemon_fork()) < 0) {
        /* Exit on error */
        daemon_retval_done();
    } else if (pid) { /* The parent */
        int ret = 0;

        /* Wait for 20 seconds for the return value passed from the daemon process */
        if ((ret = daemon_retval_wait(20)) < 0) {
            daemon_log(LOG_ERR, "Could not recieve return value from daemon process: %s", strerror(errno));
            exit(0);
        }

        switch (ret)
        {
            case 0:
                daemon_log(LOG_INFO, "Sucessfully started daemon ...");
                break;
            case 1:
                daemon_log(LOG_ERR, "Failed to close all file descriptors: %s", strerror(errno));
                break;
            default:
                daemon_log(ret != 0 ? LOG_ERR : LOG_INFO, "Daemon returned %i as return value.", ret);
                break;
        }
        exit(0);

    } else { /* The daemon */
        /* Close FDs */
        if (daemon_close_all(-1) < 0) {
            /* Send the error condition to the parent process */
            daemon_retval_send(1);
        }

        /* Send OK to parent process */
        daemon_retval_send(0);

    }
}

}

