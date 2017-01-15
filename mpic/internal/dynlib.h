#pragma once

#include "mpic/inner_pre.h"

namespace mpic {

// Dynamic library. It is used load and find symbol in the library.
// @remark You should keep the object alive before unload the loaded library.
class DynLib {
public:
#ifdef H_OS_WINDOWS
    typedef HMODULE Handler;
#else
    typedef void* Handler;
#endif

    // @param dll_path The full path for a library. e.g. "e:/project/hph/bin/test.dll", "/root/bin/test.so"
    DynLib(const std::string& dll_path);
    ~DynLib();

    // Actively load the library.
    // @return false if failed to load.
    bool Load();

    // Unload the library.
    // @return false if failed to unload
    bool Unload();

    // Query whether the library has been loaded.
    bool IsLoaded();

    // Get full path of the library
    const std::string& path(void) const {
        return dll_path_;
    }

    // Gets symbol in the library.
    // @return NULL if find nothing.
    void* GetSymbol(const std::string& func_name);

    // Gets the last loading error. It is used get the error message
    // when failed to load or unload library.
    const std::string& GetLastError() const {
        return error_;
    }

private:
    Handler handler_;
    std::string dll_path_;
    std::string error_;

private:
    // Generate library load error. It generated from system.
    std::string GetError();
};

}