#pragma once

#include <string>

#include "VulkanRegistrar.h"


// forward declaration
struct VkPipelineShaderStageCreateInfo;
struct VkPipelineVertexInputStateCreateInfo;
struct VkPipelineInputAssemblyStateCreateInfo;
struct VkPipelineViewportStateCreateInfo;
struct VkPipelineRasterizationStateCreateInfo;
struct VkPipelineMultisampleStateCreateInfo;
struct VkPipelineColorBlendAttachmentState;
struct VkPipelineColorBlendStateCreateInfo;
struct VkPipelineDynamicStateCreateInfo;
struct VkPipelineLayoutCreateInfo;

namespace aa
{
    
    // forward declaration
    class VKShader;


    // TODO
    class VKPipeline
    {

    public:
        VKPipeline(
            const VkPipelineLayoutCreateInfo& layoutCreateInfo,
                  VkGraphicsPipelineCreateInfo& pipelineCreateInfo
        );
        ~VKPipeline();

        const VkPipelineLayout vkPipelineLayout;
        const VkPipeline       vkPipeline;
        
        //  delete all implicit constructors 
        VKPipeline(const VKPipeline&)   = delete;
        VKPipeline(VKPipeline&&)        = delete;

        VKPipeline& operator = (const VKPipeline&)  = delete;
        VKPipeline& operator = (VKPipeline&&)       = delete;


    private:
        VkPipeline       createPipeline(
                  VkGraphicsPipelineCreateInfo&
        ) const;

        VkPipelineLayout createPipelineLayout(
            const VkPipelineLayoutCreateInfo& createInfo
        ) const;

    };


    // pure virtual class, acting as an interface for pipeline builders
    class IVKPipelineBuilder
    {

    public:
        IVKPipelineBuilder();

        virtual VKPipeline* build() const = 0;

        //  delete all implicit constructors 
        IVKPipelineBuilder(const IVKPipelineBuilder&)   = delete;
        IVKPipelineBuilder(IVKPipelineBuilder&&)        = delete;

        IVKPipelineBuilder& operator = (const IVKPipelineBuilder&)  = delete;
        IVKPipelineBuilder& operator = (IVKPipelineBuilder&&)       = delete;


    };


    // generic in-memory vulkan pipeline builder
    class VKPipelineBuilder final : IVKPipelineBuilder
    {
    public:
        VKPipelineBuilder();

        // lazy operation
        VKPipelineBuilder& setVertexShader(VKShader* shader);

        // lazy operation
        VKPipelineBuilder& setFragmentShader(VKShader* shader);

        VKPipeline* build() const;


    private:
        VKShader* vertexShader;
        VKShader* fragmentShader;

        void createVertexShader(
            VkPipelineShaderStageCreateInfo*        createInfo
        ) const;
        
        void createFragmentShader(
            VkPipelineShaderStageCreateInfo*        createInfo
        ) const;
        
        void createVertexInputInfo(
            VkPipelineVertexInputStateCreateInfo*   createInfo
        ) const;

        void createInputAssembly(
            VkPipelineInputAssemblyStateCreateInfo* createInfo
        ) const;

        void createViewportState(
            VkPipelineViewportStateCreateInfo*      createInfo
        ) const;

        void createRasterizer(
            VkPipelineRasterizationStateCreateInfo* createInfo
        ) const;

        void createMultisampleState(
            VkPipelineMultisampleStateCreateInfo*   createInfo
        ) const;

        void createColorBlendAttach(
            VkPipelineColorBlendAttachmentState*    createInfo
        ) const;

        void createColorBlendState(
            VkPipelineColorBlendStateCreateInfo*    createInfo,
            VkPipelineColorBlendAttachmentState*    colorBlendAttachment
        ) const;

        void createDynamicState(
            VkPipelineDynamicStateCreateInfo*       createInfo
        ) const;

        void createPipelineLayout(
            VkPipelineLayoutCreateInfo*             createInfo
        ) const;

    };
}
