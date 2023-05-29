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


    protected:
        // this shader must be managed by a module above
        VKPipeline* pipeline;

        VkBuffer          vertexBuffer;
        VkDeviceMemory    vertexBufferMemory;
        VkBuffer          indexBuffer;
        VkDeviceMemory    indexBufferMemory;

        VkBuffer		  instanceStagingBuffer;
        VkDeviceMemory	  instanceStagingBufferMemory;
        void*             instanceDataMapper;

        float lifespan;

    private:
        void createVertexBuffer();
        void createIndexBuffer();

    };


    // implements instancing in vulkan through uniform arrays
    class VKInstancedMesh : public VKMesh 
    {
    
    public:
        VKInstancedMesh(
            LogicObject* parent,
            VKPipeline*  pipeline,
            uint32_t     noInstances
        );
        virtual ~VKInstancedMesh();

        // must be called after all instances' draws
        virtual void draw() override;

        virtual void init() override;


    private:
        std::vector <float> projectionData;
        uint32_t            noInstances;

        VkBuffer            instanceBuffer;
        VkDeviceMemory      instanceBufferMemory;
        
        friend class VKMeshInstance;

    };


    // a single instance of a VKInstacedMesh
    class VKMeshInstance : public Mesh
    {
        
    public:
        VKMeshInstance(
            VKInstancedMesh* parent,
            Vector3d         position,
            uint32_t         instanceIndex,
            float            scale = 1.0f
        );
        virtual ~VKMeshInstance();

        virtual void draw() override;


    private:
        uint32_t instanceIndex;
        float    scale;

    };
}
