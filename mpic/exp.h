#pragma once

#include "platform_config.h"
#include "mpic/module.h"
#include "mpic/option.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib, "glog.lib")
#pragma comment(lib, "mpic_static.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif
