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


    // implements instancing in opengl through uniform arrays
    class GLInstancedMesh : public GLMesh
    {

    public:
        GLInstancedMesh(
            LogicObject* parent,
            GLShader*    shader,
            uint32_t     noInstances
        );
        virtual ~GLInstancedMesh();

        // must be called after all instances' draws
        virtual void draw() override;

        virtual void init() override;


    private:
        std::vector <float> projectionData;
        uint32_t            noInstances;

        unsigned int        instanceVBO;

        friend class GLMeshInstance;

    };


    // a single instance of a GLInstancedMesh
    class GLMeshInstance : public Mesh
    {

    public:
        GLMeshInstance(
            GLInstancedMesh* parent,
            Vector3d         position,
            uint32_t         instanceIndex,
            float            scale
        );
        virtual ~GLMeshInstance();

        virtual void draw() override;


    private:
        uint32_t instanceIndex;
        float    scale;

    };

}
