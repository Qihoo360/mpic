#pragma once

#include "platform_config.h"
#include "mpic/module.h"
#include "mpic/option.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib,"libglog_static.lib")
H_LINK_LIB("libmpic")
H_LINK_LIB("Ws2_32")
#endif
