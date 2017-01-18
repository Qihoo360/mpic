#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include <gflags/gflags.h>

#include "mpic/exp.h"
#include "mpic/master.h"
#include "mpic/title.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib,"libmpic.lib")
#endif

DEFINE_int32(http_port, 80, "The listening port of the http server");

class HttpOption : public mpic::Option {
public:
    HttpOption() {}
    virtual bool Init(int argc, char** argv) {
        http_port_ = FLAGS_http_port;
        return mpic::Option::Init(argc, argv);
    }

    int http_port() const {
        return http_port_;
    }

private:
    int http_port_;
};


void sigterm(int c) {
    std::string title_prefix = mpic::Option::GetExeName() + "(" + mpic::Master::instance().option()->name() + "): worker process is shutting down ...";
    mpic::Title::Set(title_prefix);
    sleep(5); // so we can use 'ps' to watch the status fo this process when it is shutting down.
    exit(0);
}


// Run on Windowns:
//      $ cd msvc/bin/Debug
//      $ ./mpic-framework.exe -config_file=mpic.conf -module_file=libechomod.dll
//
// Run on Linux
//
int main(int argc, char* argv[]) {
    signal(SIGTERM, &sigterm);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    std::shared_ptr<mpic::Option> op(new HttpOption);
    mpic::Master& pm = mpic::Master::instance();
    if (pm.Init(argc, argv, op)) {
        return pm.Run();
    } else {
        LOG(ERROR) << "master init failed.";
        return -1;
    }
}
