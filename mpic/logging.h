#pragma once

#ifdef __cplusplus
#define GOOGLE_GLOG_DLL_DECL           // If we use the static library, we must define this micro
#define GLOG_NO_ABBREVIATED_SEVERITIES // compiling will fail if we don't define this micro
#include <glog/logging.h>
#endif
