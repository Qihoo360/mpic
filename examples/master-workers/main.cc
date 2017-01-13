#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "mpic/master.h"
#include "mpic/title.h"

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


int RunWorker(const HttpOption* option) {
    LOG(INFO) << __func__ << " running ...";
    for (;;) {
        LOG(INFO) << __func__ << " pid=" << getpid() << " running http-port=" << option->http_port();
        google::FlushLogFiles(0);
        sleep(1);
    }

    return 0;
}


void sigterm(int c) {
    std::string title_prefix = mpic::Option::GetExeName() + "(" + mpic::Master::instance().option()->name() + "): worker process is shutting down ...";
    mpic::Title::Set(title_prefix);
    sleep(5); // so we can use 'ps' to watch the status fo this process when it is shutting down.
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGTERM, &sigterm);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    std::shared_ptr<mpic::Option> op(new HttpOption);
    mpic::Master& pm = mpic::Master::instance();
    if (pm.Init(argc, argv, op)) {
        return pm.Run(std::bind(&RunWorker, static_cast<HttpOption*>(op.get())));
    } else {
        return -1;
    }
}
