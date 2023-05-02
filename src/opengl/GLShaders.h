#pragma once

//  cpp includes
#include <string>
#include <vector>

//  dev includes
#include "domain/Object3d.h"
#include "data/Matrix4d.hpp"
#include "data/Vector4d.h"
#include "data/Vector3d.h"


namespace aa
{

    // forward declaration
    class GLTexture;


    // base class wrapping common openGL shaders pipeline (vertex, fragment shaders and uniforms)
    // create new objects using IGLShaderBuilders
    class GLShader {

    public:
        ~GLShader();

        void bind();

        // lazy operation until shader is bound
        void addUniformTex(
            const std::string& uniformKey,
            GLTexture*         tex
        );
        
        // lazy operation until shader is bound
        void addUniform3f(
            const std::string& uniformKey,
            Vector3d           value
        );

        // lazy operation until shader is bound
        void addUniform4f(
            const std::string& uniformKey,
            Vector4d           value
        );

        // lazy operation until shader is bound
        void addUniformMat4f(
            const std::string& uniformKey,
            Matrix4d           value
        );

        //  delete all implicit constructors 
        GLShader(const GLShader&)   = delete;
        GLShader(GLShader&&)        = delete;

        GLShader& operator = (const GLShader&)  = delete;
        GLShader& operator = (GLShader&&)       = delete;


    protected:
        GLShader();

        unsigned int shaderProgram;

        struct UniformTex {
            std::string     uniformKey;
            GLTexture*      value;
        };
        struct Uniform4f {
            std::string     uniformKey;
            Vector4d        value;
        };
        struct Uniform3f {
            std::string     uniformKey;
            Vector3d        value;
        };
        struct UniformMat4f {
            std::string     uniformKey;
             Matrix4d       value;
        };

        std::vector <UniformTex>    uniformsTex;
        std::vector <Uniform4f>     uniforms4f;
        std::vector <Uniform3f>     uniforms3f;
        std::vector <UniformMat4f>  uniformsMat4f;

        static unsigned int compileShader(
            unsigned int glType, 
            const std::string& shader
        );


    private:
        template <typename wrapperT>
        void updateOrPutIfExists(
            std::vector <wrapperT> &container,
            const wrapperT &value
        );

        friend class GLShaderFileBuilder;

    };


    // pure virtual class, acting as an interface for shader builders
    class IGLShaderBuilder
    {

    public:
        IGLShaderBuilder();

        virtual GLShader* build() = 0;

        //  delete all implicit constructors 
        IGLShaderBuilder(const IGLShaderBuilder&) = delete;
        IGLShaderBuilder(IGLShaderBuilder&&) = delete;

        IGLShaderBuilder& operator = (const IGLShaderBuilder&) = delete;
        IGLShaderBuilder& operator = (IGLShaderBuilder&&) = delete;


    };


    // shader file builder with eager and lazy operations
    class GLShaderFileBuilder final : IGLShaderBuilder
    {

    public:
        // shadersDirectory is the base directory for all files read by the builder
        GLShaderFileBuilder(const std::string& shadersDirectory);
        ~GLShaderFileBuilder();

        // lazy operation; expected location is at shadersDirectory
        GLShaderFileBuilder& setVertexShader(
            const std::string& filename
        );

        // lazy operation; expected location is at shadersDirectory
        GLShaderFileBuilder& setFragmentShader(
            const std::string& filename
        );

        // eager operation
        GLShaderFileBuilder& addUniformTex(
            const std::string&  uniformKey,
            GLTexture*          tex
        );

        // eager operation
        GLShaderFileBuilder& addUniform3f(
            const std::string& uniformKey,
            Vector3d           value
        );

        // eager operation
        GLShaderFileBuilder& addUniform4f(
            const std::string&  uniformKey,
            Vector4d            value 
        );

        virtual GLShader* build();

        const std::string shadersDirectory;


    private:
        GLShader* shaderOutput;

        std::string vertexFilepath;
        std::string fragmentFilepath;

    };

}
