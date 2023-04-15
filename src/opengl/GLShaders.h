#pragma once


//  cpp includes
#include <string>

//  dev includes
#include "domain/Object3d.h"


namespace aa
{
    //  static class contains shader types and wraps shader operations from OpenGL
    class GLShaders final
    {

    public:
        enum ShaderType {
            GLSVertex,
            GLSFragment
        };

        static unsigned int compileShader(
            ShaderType type, 
            const std::string& shader
        );

        static unsigned int createShader(
            const std::string& vertexShader, 
            const std::string& fragmentShader
        );

        static unsigned int readShader(
            const std::string& vertexName, 
            const std::string& fragmentName
        );

        //  delete all implicit constructors 
        GLShaders()                 = delete;
        GLShaders(const GLShaders&) = delete;
        GLShaders(GLShaders&&)      = delete;

        GLShaders& operator = (const GLShaders&)    = delete;
        GLShaders& operator = (GLShaders&&)         = delete;


    private:

    };
}
