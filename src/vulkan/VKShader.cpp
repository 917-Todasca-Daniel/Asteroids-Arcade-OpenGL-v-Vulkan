#include "VKShader.h"

#include <iostream>

#include "VulkanRegistrar.h"


using namespace aa;


VKShader::VKShader(const std::vector <char>& spirvCode)
    : vkShaderModule(createShaderModule(spirvCode)) { }

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



VKVertexShader::VKVertexShader(const std::vector <char>& spirvCode)
    : VKShader(spirvCode), bindingDescription(VkVertexInputBindingDescription{}) {
    bindingDescription.binding      = 0;
    bindingDescription.stride       = 0;
    bindingDescription.inputRate    = VK_VERTEX_INPUT_RATE_VERTEX;
}

VKVertexShader::~VKVertexShader() {

}


const VkVertexInputBindingDescription& VKVertexShader::getBindingDescription() {
    return bindingDescription;
}

const std::vector <VkVertexInputAttributeDescription>& VKVertexShader::getAttributeDescriptions() {
    return attributeDescriptions;
}
