    #pragma once

#include <vector>

#include "VulkanRegistrar.h"


namespace aa
{

    // wrapper over VKShaderModule, may be used for both vertex and fragment shaders
    class VKShader
    {

    public:
        VKShader(const std::vector <char>& spirvCode);
        virtual ~VKShader();

        const VkShaderModule vkShaderModule;

        //  delete all implicit constructors 
        VKShader(const VKShader&)   = delete;
        VKShader(VKShader&&)        = delete;

        VKShader& operator = (const VKShader&)  = delete;
        VKShader& operator = (VKShader&&)       = delete;


    private:
        VkShaderModule createShaderModule(const std::vector <char>& spirvCode);

    };


    // concrete wrapper over vertex shaders, allows for binding to uniforms
    class VKVertexShader : public VKShader
    {
    public:
        VKVertexShader(const std::vector <char>& spirvCode);
        virtual ~VKVertexShader();

        template <typename uniformType>
        void addBinding();

        const VkVertexInputBindingDescription&                 getBindingDescription();
        const std::vector <VkVertexInputAttributeDescription>& getAttributeDescriptions();


    private:
        VkVertexInputBindingDescription                 bindingDescription;
        std::vector <VkVertexInputAttributeDescription> attributeDescriptions;

    };


    template <typename uniformType>
    void VKVertexShader::addBinding() {
        VkVertexInputAttributeDescription attrDescription;

        attrDescription.binding = 0;
        attrDescription.location = (uint32_t)attributeDescriptions.size();
        attrDescription.format = VK_FORMAT_R32_SFLOAT;
        attrDescription.offset = bindingDescription.stride;

        bindingDescription.stride += sizeof(uniformType);

        attributeDescriptions.push_back(attrDescription);

    }
}
