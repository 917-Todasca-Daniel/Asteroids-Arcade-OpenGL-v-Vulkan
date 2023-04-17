#include "GLTexture.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	3rd parties
#include "stb_image.h"

//  dev code
#include "util/UFile.h"

using namespace aa;



GLTexture::GLTexture() : texture(0), width(0), height(0), n_channels(0)
{

}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &texture);
}


void GLTexture::bindToSlot(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void GLTexture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}



IGLTextureBuilder::IGLTextureBuilder()
{

}



GLTextureFileBuilder::GLTextureFileBuilder(const std::string& dir) 
	: IGLTextureBuilder(), texturesDirectory(UFile::normalizedDirectory(dir))
{
    if (texturesDirectory.empty()) {
        std::cout << "GLTextureFileBuilder::texturesDirectory is empty!\n";
    }
}


GLTextureFileBuilder& GLTextureFileBuilder::setColorFile(
    const std::string& filename,
    const std::string& extension
) {
    this->colorFilepath  = texturesDirectory;
    this->colorFilepath += filename;
    this->colorFilepath += '.';
    this->colorFilepath += extension;

    return *this;
}

GLTexture* GLTextureFileBuilder::build() const {
    GLTexture* output = new GLTexture();

    {   // read color map
        stbi_set_flip_vertically_on_load(1);
        unsigned char* stbi_buffer = stbi_load(
            colorFilepath.c_str(),
            &output->width,
            &output->height,
            &output->n_channels,
            4
        );

        glGenTextures(1, &output->texture);
        glBindTexture(GL_TEXTURE_2D, output->texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8,
            output->width, output->height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, stbi_buffer
        );

        glBindTexture(GL_TEXTURE_2D, 0);

        if (stbi_buffer) {
            stbi_image_free(stbi_buffer);
        }
    }

    return output;
}
