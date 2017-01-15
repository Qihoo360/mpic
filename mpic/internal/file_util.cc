
#include "file_util.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "../platform_config.h"

namespace mpic {

bool FileUtil::IsFileExist(const std::string& path) {
    if (access(path.c_str(), F_OK) != 0) {
        return false;
    }
    return true;
}

bool FileUtil::IsReadable(const std::string& strFileNmae) {
    if (access(strFileNmae.c_str(), R_OK) != 0) {
        return false;
    }
    return true;
}

std::string FileUtil::GetFileNameWithoutExt(const std::string& path) {
    size_t iStart = path.find_last_of((std::string::value_type)'/');

    if (iStart == std::string::npos) {
        iStart = 0;
    } else {
        // skip the '/'
        ++iStart;
    }

    return path.substr(iStart, path.find_last_of((std::string::value_type)'.') - iStart);
}


bool FileUtil::IsDir(const std::string& path) {
    struct stat st;
    if (0 != ::stat(path.c_str(), &st)) {
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        return true;
    }

    return false;
}

std::string FileUtil::RealPath(const std::string& path) {
#ifdef _WIN32
    return path; // TODO FIX
#else
    assert(!path.empty());
    char buf[4096];
    char* p = realpath(path.c_str(), buf);
    if (p == NULL) {
        std::cout << "convert relative path: " << path
            << " to real path error: " << strerror(errno);
    }

    return std::string(p);
#endif
}

}

