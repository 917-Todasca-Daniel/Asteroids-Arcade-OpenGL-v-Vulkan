#include "UFile.h"

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


std::string UFile::GLShaderPath(const std::string& shaderName) {
    std::string filename = ROOT;

    filename += "res/opengl/shaders/";
    filename += shaderName;
    filename += ".glsl";

    return filename;
}
