#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "mpic/exp.h"
#include "mpic/master.h"

#include "main-inl.h"

// DEFINE_string(http_port, "8080,8090", "The listening ports of the http server. We can give more than 1 port using comma to separate them.");
// DEFINE_int32(tcp_port, 8081, "The listening port of the tcp server.");
// DEFINE_string(udp_port, "5353", "The listening ports of the udp server. We can give more than 1 port using comma to separate them.");
// DEFINE_int32(tcp_thread_pool_size, 12, "The thread number in the tcp server's working thread pool");
// DEFINE_int32(http_thread_pool_size, 12, "The thread number in the http server's working thread pool");

// Run on Windowns:
//      $ cd msvc/bin/Debug
//      $ ./nfmpic.exe -config_file=mpic.conf -module_file=libnfmpic-module-echo.dll
//
// Run on Linux
//      $ ./nfmpic -module_file=/home/weizili/git/mpic/apps/modules/echo/libechomodule.so -foreground
//
int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    std::shared_ptr<mpic::Option> op(new mpic::Option);
    mpic::Master& pm = mpic::Master::instance();
    if (pm.Init(argc, argv, op)) {
        return pm.Run();
    } else {
        LOG(ERROR) << "master init failed.";
        return -1;
    }
}
