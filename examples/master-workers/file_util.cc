
#include "file_util.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace mpic {

bool FileUtil::IsFileExist(const std::string& strFileName) {
    if (access(strFileName.c_str(), F_OK) != 0) {
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

std::string FileUtil::GetFileNameWithoutExt(const std::string& strFileName) {
    // since '/' equal to L'/', we can cast to either one.
    std::string::size_type iStart = strFileName.find_last_of((std::string::value_type)'/');

    if (iStart == std::string::npos) {
        iStart = 0;
    } else {
        // skip the '/'
        ++iStart;
    }

    return strFileName.substr(iStart, strFileName.find_last_of((std::string::value_type)'.') - iStart);
}


bool FileUtil::IsDir(const std::string& strFileName) {
    struct stat st;
    if (0 != stat(strFileName.c_str(), &st)) {
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        return true;
    }

    return false;
}


}

