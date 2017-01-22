#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "nfmpic_option.h"

#include "mpic/exp.h"
#include "mpic/master.h"
#include "mpic/title.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib,"libmpic.lib")
#endif


void sigterm(int c) {
    std::string title_prefix = mpic::Option::GetExeName() + "(" + mpic::Master::instance().option()->name() + "): worker process is shutting down ...";
    mpic::Title::Set(title_prefix);
    exit(0);
}


// Run on Windowns:
//      $ cd msvc/bin/Debug
//      $ ./nfmpic.exe -config_file=mpic.conf -module_file=libechomod.dll
//
// Run on Linux
//
int main(int argc, char* argv[]) {
    //signal(SIGTERM, &sigterm);
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
