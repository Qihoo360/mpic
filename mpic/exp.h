#pragma once

#include "platform_config.h"
#include "mpic/module.h"
#include "mpic/option.h"

#ifdef H_OS_WINDOWS
H_LINK_LIB("libmpic")
H_LINK_LIB("libglog")
H_LINK_LIB("Ws2_32")
H_LINK_LIB("gflags")
H_LINK_LIB("ShLwApi")
#endif
