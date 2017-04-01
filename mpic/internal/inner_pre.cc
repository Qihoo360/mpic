#include "inner_pre.h"
#include "../logging.h"

#ifdef H_OS_WINDOWS
//#pragma comment(lib,"ShLwApi.Lib") // Fix gflags link problem : error LNK2019: unresolved external symbol __imp__PathMatchSpecA@8 referenced in function "public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > __thiscall google::`anonymous namespace'::CommandLineFlagParser::ProcessOptionsFromStringLocked(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,enum google::FlagSettingMode)" (?ProcessOptionsFromStringLocked@CommandLineFlagParser@?A0x75064daa@google@@QAE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ABV45@W4FlagSettingMode@3@@Z)
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"glog.lib")
#endif

namespace mpic {

namespace {
struct OnStartup {
    OnStartup() {
#ifndef H_OS_WINDOWS
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            LOG(ERROR) << "SIGPIPE set failed.";
            exit(-1);
        }
        LOG(INFO) << "ignore SIGPIPE";
#endif
    }
    ~OnStartup() {
    }
} __s_onstartup;
}

}
