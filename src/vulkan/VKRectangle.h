#pragma once

#include "domain/Object3d.h"

#include "VulkanRegistrar.h"


namespace aa
{

    class VKPipeline;


    // draws a static 2d rectangle to the screen;
    // the given position is the center of the rectangle relative to the WINDOW_UNIT
    // pipeline's vertex buffer expects 2d coordinates
    class VKRectangle : public Object3d
    {

    public:
        VKRectangle(
            LogicObject*    parent,
            Vector3d        position,
            float           height,
            float           width,
            VKPipeline*     pipeline
        );
        virtual ~VKRectangle();

        virtual void init();

        virtual void loop(float lap);

        virtual void draw();


    private:
        float       width;
        float       height;
        float       vertices2d[8] = {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
        };
        uint32_t    indices2d[6] = { 0, 1, 2, 2, 3, 0 };
        VKPipeline* pipeline;

        VkBuffer          vertexBuffer;
        VkDeviceMemory    vertexBufferMemory;
        VkBuffer          indexBuffer;
        VkDeviceMemory    indexBufferMemory;

        float uniformValue;

        void createVertexBuffer();
        void createIndexBuffer ();

    };
}
