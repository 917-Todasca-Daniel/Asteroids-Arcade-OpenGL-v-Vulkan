#pragma once

#include "domain/Mesh.h"

#include <vector>


namespace aa
{

    class GLShader;
    class GLTexture;
    

    // OpenGL concrete implementation of the aa::Mesh interface
    // shader expects position, normal vector, and texture coordinate
    class GLMesh : public Mesh
    {

    public:
        GLMesh(
            LogicObject*    parent,
            Vector3d        position,
            GLShader*       shader
        );
        virtual ~GLMesh();

        void loadFromFbx(const char* filepath, float scale = 1.0f);

        virtual void init();

        virtual void draw();


    protected:
        // this shader must be managed by a module above
        GLShader* shader;

        // vertex buffer object
        unsigned int vbo;
        // index buffer object
        unsigned int ibo;

    };

}
