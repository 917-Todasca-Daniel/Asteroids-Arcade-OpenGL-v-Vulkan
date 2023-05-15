#include "UFile.h"

#include <algorithm>

#include <assimp/Importer.hpp>

using namespace aa;


const std::string UFile::ROOT = "D:/licenta/dev/app/";

Assimp::Importer* UFile::IMPORTER = nullptr;


std::string UFile::readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::string contents;
    std::getline(file, contents, '\0');

    return contents;
}


std::vector <char> UFile::readBinaryFileContent(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
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


Assimp::Importer* UFile::getAssimpFileReader() {
    return IMPORTER;
}

void UFile::setAssimpFileReader(Assimp::Importer* importer) {
    IMPORTER = importer;
}
