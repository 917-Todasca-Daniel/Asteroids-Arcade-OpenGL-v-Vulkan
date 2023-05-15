#include "VKPipeline.h"

#include "VulkanRegistrar.h"

#include "VKShader.h"

#include <iostream>

// dev code
#include "util/UFile.h"

using namespace aa;


VKPipeline::VKPipeline(
    const VkPipelineLayoutCreateInfo&   layoutCreateInfo,
          VkGraphicsPipelineCreateInfo& pipelineCreateInfo
) : vkPipelineLayout(createPipelineLayout(layoutCreateInfo)), 
    vkPipeline(createPipeline(pipelineCreateInfo)) {

}

VKPipeline::~VKPipeline() {
    vkDestroyPipeline       (VK_DEVICE, vkPipeline,         nullptr);
    vkDestroyPipelineLayout (VK_DEVICE, vkPipelineLayout,   nullptr);
}


VkPipelineLayout VKPipeline::createPipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const {
    VkPipelineLayout vkPipelineLayout{ };

    auto ret = vkCreatePipelineLayout(VK_DEVICE, &createInfo, nullptr, &vkPipelineLayout);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create pipeline layout!\n";
    }

    return vkPipelineLayout;
}

VkPipeline VKPipeline::createPipeline(VkGraphicsPipelineCreateInfo& pipelineCreateInfo) const {
    VkPipeline vkPipeline{ };

    pipelineCreateInfo.layout = vkPipelineLayout;

    if (vkCreateGraphicsPipelines(
        VK_DEVICE, 
        VK_NULL_HANDLE, 
        1, 
        &pipelineCreateInfo,
        nullptr, 
        &vkPipeline
    ) != VK_SUCCESS) {
        std::cout << "Failed to create graphics pipeline!\n";;
    }

    return vkPipeline;
}



IVKPipelineBuilder::IVKPipelineBuilder() { }



VKPipelineBuilder::VKPipelineBuilder() : vertexShader(nullptr), fragmentShader(nullptr) {

}


VKPipelineBuilder& VKPipelineBuilder::setVertexShader(VKShader* shader)
{
    this->vertexShader = shader;
    return *this;
}

VKPipelineBuilder& VKPipelineBuilder::setFragmentShader(VKShader* shader)
{
    this->fragmentShader = shader;
    return *this;
}

VKPipeline* VKPipelineBuilder::build() const
{
    if (vertexShader == nullptr) {
        std::cout << "Pipeline vertex shader not found!\n";
        return nullptr;
    }
    if (fragmentShader == nullptr) {
        std::cout << "Pipeline vertex shader not found!\n";
        return nullptr;
    }

    VkPipelineVertexInputStateCreateInfo    vertexInputInfo     { };
    VkPipelineInputAssemblyStateCreateInfo  inputAssembly       { };
    VkPipelineViewportStateCreateInfo       viewportState       { };
    VkPipelineRasterizationStateCreateInfo  rasterizer          { };
    VkPipelineMultisampleStateCreateInfo    multisampling       { };
    VkPipelineColorBlendAttachmentState     colorBlendAttachment{ };
    VkPipelineColorBlendStateCreateInfo     colorBlending       { };
    VkPipelineDynamicStateCreateInfo        dynamicState        { };
    VkPipelineLayoutCreateInfo              pipelineLayoutInfo  { };

    VkPipelineShaderStageCreateInfo         shaderStages[2] = {
        VkPipelineShaderStageCreateInfo{ },
        VkPipelineShaderStageCreateInfo{ }
    };

    createVertexShader      (&shaderStages[0]);
    createFragmentShader    (&shaderStages[1]);
    createVertexInputInfo   (&vertexInputInfo);
    createInputAssembly     (&inputAssembly);
    createRasterizer        (&rasterizer);
    createMultisampleState  (&multisampling);
    createColorBlendAttach  (&colorBlendAttachment);
    createColorBlendState   (&colorBlending, &colorBlendAttachment);
    createDynamicState      (&dynamicState);
    createPipelineLayout    (&pipelineLayoutInfo);

    VkGraphicsPipelineCreateInfo pipelineInfo{ };
    pipelineInfo.sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount                 = 2;
    pipelineInfo.pStages                    = shaderStages;
    pipelineInfo.pVertexInputState          = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState        = &inputAssembly;
    pipelineInfo.pViewportState             = &viewportState;
    pipelineInfo.pRasterizationState        = &rasterizer;
    pipelineInfo.pMultisampleState          = &multisampling;
    pipelineInfo.pDepthStencilState         = nullptr; 
    pipelineInfo.pColorBlendState           = &colorBlending;
    pipelineInfo.pDynamicState              = &dynamicState;
    pipelineInfo.renderPass           = VK_RENDER_PASS;
    pipelineInfo.subpass              = 0;
    pipelineInfo.basePipelineHandle   = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex    = -1;

    VKPipeline* vkPipeline = new VKPipeline(pipelineLayoutInfo, pipelineInfo);

    return vkPipeline;
}


void VKPipelineBuilder::createVertexShader(VkPipelineShaderStageCreateInfo* createInfo) const
{
    createInfo->sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo->stage   = VK_SHADER_STAGE_VERTEX_BIT;
    createInfo->module  = vertexShader->vkShaderModule;
    createInfo->pName   = "main";
}

void VKPipelineBuilder::createFragmentShader(VkPipelineShaderStageCreateInfo* createInfo) const
{   
    createInfo->sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo->stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo->module  = fragmentShader->vkShaderModule;
    createInfo->pName   = "main";
}

void VKPipelineBuilder::createVertexInputInfo(VkPipelineVertexInputStateCreateInfo* createInfo) const
{   // how to read vertices from the vertex buffer
    createInfo->sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo->vertexBindingDescriptionCount   = 0;
    createInfo->pVertexBindingDescriptions      = nullptr;
    createInfo->vertexAttributeDescriptionCount = 0;
    createInfo->pVertexAttributeDescriptions    = nullptr;
}

void VKPipelineBuilder::createInputAssembly(VkPipelineInputAssemblyStateCreateInfo* createInfo) const
{   // type of geometry to be drawn
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo->topology                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    createInfo->primitiveRestartEnable  = VK_FALSE;
}

void VKPipelineBuilder::createViewportState(VkPipelineViewportStateCreateInfo* createInfo) const
{   // no of viewports (dynamic data)
    createInfo->sType           = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo->viewportCount   = 1;
    createInfo->scissorCount    = 1;
}

void VKPipelineBuilder::createRasterizer(VkPipelineRasterizationStateCreateInfo* createInfo) const
{
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo->depthClampEnable        = VK_FALSE;
    createInfo->rasterizerDiscardEnable = VK_FALSE;
    createInfo->polygonMode             = VK_POLYGON_MODE_FILL;
    createInfo->lineWidth               = 1.0f;
    createInfo->cullMode                = VK_CULL_MODE_BACK_BIT;
    createInfo->frontFace               = VK_FRONT_FACE_CLOCKWISE;
    createInfo->depthBiasEnable         = VK_FALSE;
    createInfo->depthBiasConstantFactor = 0.0f; 
    createInfo->depthBiasClamp          = 0.0f; 
    createInfo->depthBiasSlopeFactor    = 0.0f;
}

void VKPipelineBuilder::createMultisampleState(VkPipelineMultisampleStateCreateInfo* createInfo) const
{   // multisampling disabled
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo->sampleShadingEnable     = VK_FALSE;
    createInfo->rasterizationSamples    = VK_SAMPLE_COUNT_1_BIT;
    createInfo->minSampleShading        = 1.0f;
    createInfo->pSampleMask             = nullptr; 
    createInfo->alphaToCoverageEnable   = VK_FALSE; 
    createInfo->alphaToOneEnable        = VK_FALSE; 
}

void VKPipelineBuilder::createColorBlendAttach(VkPipelineColorBlendAttachmentState* createInfo) const
{
    createInfo->colorWriteMask  = VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    createInfo->blendEnable     = VK_FALSE;
}

void VKPipelineBuilder::createColorBlendState(
    VkPipelineColorBlendStateCreateInfo* createInfo,
    VkPipelineColorBlendAttachmentState* colorBlendAttachment
) const {
    createInfo->sType               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo->logicOpEnable       = VK_FALSE;
    createInfo->logicOp             = VK_LOGIC_OP_COPY;
    createInfo->attachmentCount     = 1;
    createInfo->pAttachments        = colorBlendAttachment;
    createInfo->blendConstants[0]   = 0.0f;
    createInfo->blendConstants[1]   = 0.0f;
    createInfo->blendConstants[2]   = 0.0f;
    createInfo->blendConstants[3]   = 0.0f;
}

void VKPipelineBuilder::createDynamicState(VkPipelineDynamicStateCreateInfo* createInfo) const
{   // data that needs to be specified at every draw call
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    createInfo->sType               = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo->dynamicStateCount   = (uint32_t) (dynamicStates.size());
    createInfo->pDynamicStates      = dynamicStates.data();
}

void VKPipelineBuilder::createPipelineLayout(VkPipelineLayoutCreateInfo* createInfo) const
{   // uniforms configurations
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo->setLayoutCount          = 0;
    createInfo->pushConstantRangeCount  = 0;
}
