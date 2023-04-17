#pragma once

#include "domain/Object3d.h"


namespace aa
{

    // wrapper over OpenGL textures
    // instances may get large, using pointer references is advised
    class GLTexture
    {

    friend class GLTextureFileBuilder;

    public:
        GLTexture();
        ~GLTexture();

        void bindToSlot(unsigned int slot = 0) const;
        void unbind() const;

        //  delete all implicit constructors 
        GLTexture(const GLTexture&) = delete;
        GLTexture(GLTexture&&) = delete;

        GLTexture& operator = (const GLTexture&) = delete;
        GLTexture& operator = (GLTexture&&) = delete;


    private:
        unsigned int    texture;
        int             width;
        int             height;
        int             n_channels;

    };


    // pure virtual class, acting as an interface for texture builders
    class IGLTextureBuilder 
    {
       
    public:
        IGLTextureBuilder();

        virtual GLTexture* build() const = 0;

        //  delete all implicit constructors 
        IGLTextureBuilder(const IGLTextureBuilder&) = delete;
        IGLTextureBuilder(IGLTextureBuilder&&) = delete;

        IGLTextureBuilder& operator = (const IGLTextureBuilder&) = delete;
        IGLTextureBuilder& operator = (IGLTextureBuilder&&) = delete;


    };


    // tex file builder with lazy initialization
    class GLTextureFileBuilder : IGLTextureBuilder
    {

    public:
        GLTextureFileBuilder(const std::string& texturesDirectory);

        // expected location is at textureDirectory
        GLTextureFileBuilder& setColorFile(
            const std::string& filename,
            const std::string& extension
        );

        virtual GLTexture* build() const;

        const std::string texturesDirectory;


    private:
        std::string colorFilepath;

    };

}
