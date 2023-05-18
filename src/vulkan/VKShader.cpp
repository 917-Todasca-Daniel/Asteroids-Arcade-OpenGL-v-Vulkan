#include "VKShader.h"

#include <iostream>

#include "VulkanRegistrar.h"


using namespace aa;


VKShader::VKShader(const std::vector <char>& spirvCode) : 
    vkShaderModule(createShaderModule(spirvCode)) { }

VKShader::~VKShader() {
    vkDestroyShaderModule(*VK_DEVICE, vkShaderModule, nullptr);
}


VkShaderModule VKShader::createShaderModule(const std::vector <char>& spirvCode) {
    VkShaderModuleCreateInfo createInfo { };
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvCode.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(spirvCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(*VK_DEVICE, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        std::cout << "Failed to create shader module!\n";
    }

    return shaderModule;
}



VKVertexShader::VKVertexShader(const std::vector <char>& spirvCode) : 
    VKShader            (spirvCode),
    bindingDescription  (VkVertexInputBindingDescription{}),
    descriptorSetLayout (VkDescriptorSetLayout{}), 
    uniformBinding      (VkDescriptorSetLayoutBinding{}) {
    bindingDescription.binding      = 0;
    bindingDescription.stride       = 0;
    bindingDescription.inputRate    = VK_VERTEX_INPUT_RATE_VERTEX;
}

VKVertexShader::~VKVertexShader() {
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer (*VK_DEVICE, uniformBuffers[i], nullptr);
        vkFreeMemory    (*VK_DEVICE, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool     (*VK_DEVICE, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(*VK_DEVICE, descriptorSetLayout, nullptr);
}


void VKVertexShader::addUniform() {
    createUniformLayout();
    createUniformPool();
}

void VKVertexShader::createUniformLayout() {
    VkDescriptorSetLayoutCreateInfo layoutInfo { };
    VkDeviceSize bufferSize = sizeof(float);

    uniformBinding.binding              = 0;
    uniformBinding.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBinding.descriptorCount      = 1;
    uniformBinding.stageFlags           = VK_SHADER_STAGE_VERTEX_BIT;
    uniformBinding.pImmutableSamplers   = nullptr;
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &uniformBinding;

    if (vkCreateDescriptorSetLayout(
        *VK_DEVICE, &layoutInfo, nullptr, &descriptorSetLayout
    ) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor set layout!\n";
    }

    uniformBuffers      .resize(VK_MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(VK_MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(VK_MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
        VulkanRegistrar::createBuffer(bufferSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], 
            uniformBuffersMemory[i]
        );

        vkMapMemory(
            *VK_DEVICE, uniformBuffersMemory[i], 
            0, bufferSize, 0, &uniformBuffersMapped[i]
        );
    }
}

void VKVertexShader::createUniformPool() {
    VkDescriptorPoolSize        poolSize { };
    VkDescriptorPoolCreateInfo  poolInfo { };

    poolSize.type               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount    = static_cast<uint32_t>(VK_MAX_FRAMES_IN_FLIGHT);
    poolInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount      = 1;
    poolInfo.pPoolSizes         = &poolSize;
    poolInfo.maxSets            = static_cast<uint32_t>(VK_MAX_FRAMES_IN_FLIGHT);


    if (vkCreateDescriptorPool(
        *VK_DEVICE, &poolInfo, nullptr, &descriptorPool
    ) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor pool!\n";
    }

    std::vector<VkDescriptorSetLayout> layouts(VK_MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo { };

    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VK_MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts.data();

    descriptorSets.resize(VK_MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(*VK_DEVICE, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo    { };
        VkWriteDescriptorSet descriptorWrite { };

        bufferInfo.buffer   = uniformBuffers[i];
        bufferInfo.offset   = 0;
        bufferInfo.range    = sizeof(float);
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet           = descriptorSets[i];
        descriptorWrite.dstBinding       = 0;
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = nullptr;
        descriptorWrite.pTexelBufferView = nullptr; 

        vkUpdateDescriptorSets(*VK_DEVICE, 1, &descriptorWrite, 0, nullptr);
    }
}


const VkVertexInputBindingDescription& VKVertexShader::getBindingDescription() const {
    return bindingDescription;
}

const VkDescriptorSetLayout& VKVertexShader::getDescriptorLayout() const {
    return descriptorSetLayout;
}

VkDescriptorSet VKVertexShader::getDescriptorSet(int index) const {
    return descriptorSets[index];
}

const std::vector <VkVertexInputAttributeDescription>& 
    VKVertexShader::getAttributeDescriptions() const {
    return attributeDescriptions;
}

void* VKVertexShader::getUniformBuffersMap(int index) const {
    return uniformBuffersMapped[index];
}
