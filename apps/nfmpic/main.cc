#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "mpic/exp.h"
#include "mpic/master.h"

#include "option.h"

#include "main-inl.h"

// #ifdef H_OS_WINDOWS
// H_LINK_LIB("libglog_static")
// H_LINK_LIB("Ws2_32")
// #endif
// 
// #ifdef H_OS_WINDOWS
// #pragma comment(lib,"libmpic.lib")
// #endif

// Run on Windowns:
//      $ cd vsprojects/bin/Debug
//      $ ./nfmpic.exe --cfg=mpic.conf --mod=libnfmpic-module-echo.dll
//
// Run on Linux
//      $ touch /tmp/mpic.conf && ./nfmpic --mod=/home/weizili/git/mpic/apps/modules/echo/libechomodule.so -f
//      $ touch /tmp/mpic.conf && ../build/bin/nfmpic   --mod=/home/weizili/git/mpic/build/lib/libnfmpic_module_echo.so -f 
//
//  on another console:
//      $ curl -s http://127.0.0.1:8081/echo -d xxx
//
int main(int argc, char* argv[]) {
    std::shared_ptr<mpic::Option> op(new nfmpic::Option);
    mpic::Master& pm = mpic::Master::instance();
    if (pm.Init(argc, argv, op)) {
        return pm.Run();
    } else {
        LOG(ERROR) << "master init failed.";
        return -1;
    }
}
