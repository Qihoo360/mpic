#pragma once

#include <string>

namespace mpic {

class Title {
public:
    static bool Init(int argc, char** argv);
    static void Set(const std::string& title);
};

}

