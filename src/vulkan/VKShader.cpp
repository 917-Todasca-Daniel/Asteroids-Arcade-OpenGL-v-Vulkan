#include "VKShader.h"

#include <iostream>

#include "VulkanRegistrar.h"

#include "VKTexture.h"


using namespace aa;


VKShader::VKShader(const std::vector <char>& spirvCode) : 
    vkShaderModule(createShaderModule(spirvCode)),
    bufferRange(0.0),
    descriptorSetLayout(VkDescriptorSetLayout{}) { }

VKShader::~VKShader() {
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer (*VK_DEVICE, uniformBuffers[i],       nullptr);
        vkFreeMemory    (*VK_DEVICE, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool     (*VK_DEVICE, descriptorPool,      nullptr);
    vkDestroyDescriptorSetLayout(*VK_DEVICE, descriptorSetLayout, nullptr);

    vkDestroyShaderModule       (*VK_DEVICE, vkShaderModule,      nullptr);
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


VKShader& VKShader::addUniform(uint32_t bufferRange) {
    this->bufferRange = bufferRange;

    {   // add uniform binding
        VkDescriptorSetLayoutBinding uniformBinding{ };
        uniformBinding.binding            = (uint32_t)uniformBindings.size();
        uniformBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBinding.descriptorCount    = 1;
        uniformBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uniformBinding.pImmutableSamplers = nullptr;
        uniformBindings.push_back(uniformBinding);
    }

    {   // prepare descriptor set
        VkWriteDescriptorSet descriptorWrite{ };
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstBinding       = (uint32_t)descriptorWriteSets.size();
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pImageInfo       = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        descriptorWriteSets.push_back(descriptorWrite);
    }

    return *this;
}

VKShader& VKShader::addTextureUniform(VKTexture* tex) {
    this->tex = tex;

    {   // add uniform binding
        VkDescriptorSetLayoutBinding textureUniformBinding{ };
        textureUniformBinding.binding            = (uint32_t)uniformBindings.size();
        textureUniformBinding.descriptorCount    = 1;
        textureUniformBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureUniformBinding.pImmutableSamplers = nullptr;
        textureUniformBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
        uniformBindings.push_back(textureUniformBinding);
    }

    {   // prepare descriptor set
        VkWriteDescriptorSet descriptorWrite{ };
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstBinding       = (uint32_t)descriptorWriteSets.size();
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pImageInfo       = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        descriptorWriteSets.push_back(descriptorWrite);
    }

    return *this;
}


void VKShader::buildUniforms() {
    createUniformLayout(bufferRange);
    createUniformPool(bufferRange);
}

void VKShader::createUniformLayout(uint32_t bufferRange) {
    VkDescriptorSetLayoutCreateInfo layoutInfo{ };

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (uint32_t)uniformBindings.size();
    layoutInfo.pBindings = uniformBindings.data();

    if (vkCreateDescriptorSetLayout(
        *VK_DEVICE, &layoutInfo, nullptr, &descriptorSetLayout
    ) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor set layout!\n";
    }

    if (bufferRange > 0) {
        uniformBuffers.resize(VK_MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(VK_MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(VK_MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
            VulkanRegistrar::createBuffer(bufferRange,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i],
                uniformBuffersMemory[i]
            );

            vkMapMemory(
                *VK_DEVICE, uniformBuffersMemory[i],
                0, bufferRange, 0, &uniformBuffersMapped[i]
            );
        }
    }
}

void VKShader::createUniformPool(uint32_t bufferRange) {
    std::vector <VkDescriptorPoolSize> poolSizes;
    VkDescriptorPoolCreateInfo         poolInfo{ };

    for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
        for (auto& dw : descriptorWriteSets) {
            VkDescriptorPoolSize poolSize{ };
            if (dw.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                poolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                poolSize.descriptorCount = (uint32_t)(VK_MAX_FRAMES_IN_FLIGHT);
                poolSizes.push_back(poolSize);
            } else
            if (dw.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                poolSize.descriptorCount = (uint32_t)(VK_MAX_FRAMES_IN_FLIGHT);
                poolSizes.push_back(poolSize);
            }
        }
    }

    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = (uint32_t)(VK_MAX_FRAMES_IN_FLIGHT);


    if (vkCreateDescriptorPool(
        *VK_DEVICE, &poolInfo, nullptr, &descriptorPool
    ) != VK_SUCCESS) {
        std::cout << "Failed to create descriptor pool!\n";
    }

    std::vector<VkDescriptorSetLayout> layouts(VK_MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{ };

    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = (uint32_t)(VK_MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts.data();

    descriptorSets.resize(VK_MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(*VK_DEVICE, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        std::cout << "Failed to allocate descriptor sets!\n";
    }

    for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo   imageInfo { };
        VkDescriptorBufferInfo  bufferInfo{ };
        std::vector <VkWriteDescriptorSet> descriptorWriteSets(this->descriptorWriteSets);

        for (auto& dw : descriptorWriteSets) {
            dw.dstSet = descriptorSets[i];
            if (dw.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView   = tex->getTextureImageView();
                imageInfo.sampler     = tex->getTextureSampler();

                dw.pImageInfo         = &imageInfo;
            } else
            if (dw.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range  = bufferRange;

                dw.pBufferInfo    = &bufferInfo;
            }
        }

        vkUpdateDescriptorSets(
            *VK_DEVICE, (uint32_t)descriptorWriteSets.size(),
            descriptorWriteSets.data(), 0, nullptr
        );
    }
}


const VkDescriptorSetLayout& VKShader::getDescriptorLayout() const {
    return descriptorSetLayout;
}

VkDescriptorSet VKShader::getDescriptorSet(int index) const {
    return descriptorSets[index];
}

void* VKShader::getUniformBuffersMap(int index) const {
    return uniformBuffersMapped[index];
}



VKVertexShader::VKVertexShader(const std::vector <char>& spirvCode) : 
    VKShader            (spirvCode),
    bindingDescription  (VkVertexInputBindingDescription{}) {
    bindingDescription.binding      = 0;
    bindingDescription.stride       = 0;
    bindingDescription.inputRate    = VK_VERTEX_INPUT_RATE_VERTEX;
}

VKVertexShader::~VKVertexShader() {

}


const VkVertexInputBindingDescription& VKVertexShader::getBindingDescription() const {
    return bindingDescription;
}

const std::vector <VkVertexInputAttributeDescription>& 
    VKVertexShader::getAttributeDescriptions() const {
    return attributeDescriptions;
}
