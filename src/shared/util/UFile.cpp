#include "UFile.h"

#include <algorithm>

using namespace aa;


const std::string UFile::ROOT = "D:/licenta/dev/app/";


std::string UFile::readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::string contents;
    std::getline(file, contents, '\0');

    return contents;
}


std::string UFile::normalizedDirectory(const std::string& directory)
{
    std::string dir = directory;
    std::replace(dir.begin(), dir.end(), '\\', '/');
    if (!dir.empty() && dir.back() != '/') {
        dir.push_back('/');
    }

    return dir;
}
