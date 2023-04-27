#pragma once

#include "domain/Object3d.h"

#include <vector>


namespace aa
{

    class GLShader;
    class GLTexture;

    // TODO
    class GLMesh : public Object3d
    {

    public:
        GLMesh(
            LogicObject*    parent,
            Vector3d        position,
            GLShader*       shader
        );
        virtual ~GLMesh();

        void loadFromFbx(const char* filepath);

        virtual void init();

        virtual void draw() const;

        void setPosition(Vector3d other) {
            this->position = other;
        }

        //  delete all implicit constructors 
        GLMesh()                = delete;
        GLMesh(const GLMesh&)   = delete;
        GLMesh(GLMesh&&)        = delete;

        GLMesh& operator = (const GLMesh&)    = delete;
        GLMesh& operator = (GLMesh&&)         = delete;


    protected:
        // this shader must be managed by a module above
        GLShader* shader;

        // contains vertex position and vertex normals (for reflecting light)
        std::vector <float> vertices;

        std::vector <unsigned int> indices;

        // vertex buffer object
        unsigned int vbo;
        // index buffer object
        unsigned int ibo;

    };

}
