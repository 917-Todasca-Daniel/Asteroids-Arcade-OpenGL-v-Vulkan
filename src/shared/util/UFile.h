#pragma once


#include <string>
#include <fstream>


namespace aa
{

    class UFile {

    public:
        static std::string readFileContent(const std::string& filename);

        static std::string GLShaderPath(const std::string& shaderName);


    private:
        static const std::string ROOT;

    };

}
