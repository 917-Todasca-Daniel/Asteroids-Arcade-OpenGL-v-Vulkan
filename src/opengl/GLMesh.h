#pragma once

#include "domain/Object3d.h"

#include "data/Quaternion.hpp"

#include <vector>


namespace aa
{

    class GLShader;
    class GLTexture;

    // loads and draws 3d objects
    // no rotation is applied by default and position is additive to vertex positions
    // shader expects position, normal vector, and texture coordinate
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

        virtual void draw();

        // getters and setter
        void setPosition(Vector3d other) {
            this->position = other;
        }

        void setRotation(Quaternion rot) {
            this->rotation = rot;
        }

        Quaternion& getRotation() {
            return this->rotation;
        }

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
        // rotation around the center
        Quaternion                  rotation;

        // vertex buffer object
        unsigned int vbo;
        // index buffer object
        unsigned int ibo;

    };

}
