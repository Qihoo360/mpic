#pragma once

#include <string>
#include "mpic_export.h"

namespace mpic {

class MPIC_EXPORT Title {
public:
    static bool Init(int argc, char** argv);
    static void Set(const std::string& title);
};

}

