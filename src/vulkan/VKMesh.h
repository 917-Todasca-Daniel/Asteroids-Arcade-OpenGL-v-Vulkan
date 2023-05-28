#pragma once

#include "domain/Mesh.h"

#include "VulkanRegistrar.h"


namespace aa
{

    class VKPipeline;


    // Vulkan concrete implementation of the aa::Mesh interface
    // pipeline shader expects position, normal vector, and texture coordinates
    class VKMesh : public Mesh
    {

    public:
        VKMesh(
            LogicObject*    parent,
            Vector3d        position,
            VKPipeline*     pipeline
        );
        virtual ~VKMesh();

        void loadFromFbx(const char* filepath, float scale = 1.0f);

        virtual void loop(float lap);

        virtual void init();

        virtual void draw();


    private:
        // this shader must be managed by a module above
        VKPipeline* pipeline;

        VkBuffer          vertexBuffer;
        VkDeviceMemory    vertexBufferMemory;
        VkBuffer          indexBuffer;
        VkDeviceMemory    indexBufferMemory;

        float lifespan;

        void createVertexBuffer();
        void createIndexBuffer();

    };
}
