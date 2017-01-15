#pragma once

#include <string>

namespace mpic {

class FileUtil {
public:
    // Gets base file name. the file extension will be removed.
    // e.g. "D:/test/aab.jpg" ==> "aab"
    static std::string GetFileNameWithoutExt(const std::string& filepath);

    // Determines if a file exists and could be opened.
    // @note The file CAN be a directory
    // @param filename is the std::string identifying the file which should be tested for existence.
    // @return Returns true if file exists, and false if it does not exist or an error occurred.
    static bool IsFileExist(const std::string& filepath);

    static bool IsReadable(const std::string& filepath);

    static bool IsDir(const std::string& filepath);

    static const std::string& GetExeName();
    static std::string RealPath(const std::string& path);
};

}


