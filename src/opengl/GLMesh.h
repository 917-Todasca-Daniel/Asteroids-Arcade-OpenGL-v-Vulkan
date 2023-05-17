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

        void loadFromFbx(const char* filepath);

        virtual void init();

        virtual void draw();

        Vector3d getCenter() const {
            return center;
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

        // contains vertex position, vertex normals, tex coord (for reflecting light)
        std::vector <float>         vertices;
        std::vector <unsigned int>  indices;

        // by default, the mean average of all points
        Vector3d                    center;

        // vertex buffer object
        unsigned int vbo;
        // index buffer object
        unsigned int ibo;

    };

}
