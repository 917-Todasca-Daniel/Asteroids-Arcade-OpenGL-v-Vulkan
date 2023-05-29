    #pragma once

#include <vector>

#include "VulkanRegistrar.h"


namespace aa
{

    class VKTexture;


    // wrapper over VKShaderModule, may be used for both vertex and fragment shaders
    class VKShader
    {

    public:
        VKShader(const std::vector <char>& spirvCode);
        virtual ~VKShader();

        const VkShaderModule vkShaderModule;

        VKShader& addUniform(uint32_t bufferRange, uint32_t instanceCount = 1);
        VKShader& addTextureUniform(VKTexture* tex);
        void buildUniforms();

        uint32_t getInstanceCount() const {
            return instanceCount;
        }

        const VkDescriptorSetLayout&           getDescriptorLayout      ()           const;
              VkDescriptorSet                  getDescriptorSet         (int index)  const;
              void*                            getUniformBuffersMap     (int index)  const;

        //  delete all implicit constructors 
        VKShader(const VKShader&)   = delete;
        VKShader(VKShader&&)        = delete;

        VKShader& operator = (const VKShader&)  = delete;
        VKShader& operator = (VKShader&&)       = delete;


    protected:
        float      bufferRange;
        VKTexture* tex;
        uint32_t   instanceCount;

        std::vector<void*>                        uniformBuffersMapped;
        std::vector<VkBuffer>                     uniformBuffers;
        std::vector<VkDescriptorSetLayoutBinding> uniformBindings;
        std::vector<VkWriteDescriptorSet>         descriptorWriteSets;
        std::vector<VkDeviceMemory>               uniformBuffersMemory;

        VkDescriptorSetLayout           descriptorSetLayout;
        std::vector<VkDescriptorSet>    descriptorSets;
        VkDescriptorPool                descriptorPool;


    private:

        VkShaderModule createShaderModule(const std::vector <char>& spirvCode);

        void createUniformLayout(uint32_t bufferRange);
        void createUniformPool(uint32_t bufferRange);

    };


    // concrete wrapper over vertex shaders, allows for binding to uniforms and vertices
    class VKVertexShader : public VKShader
    {
    public:
        VKVertexShader(const std::vector <char>& spirvCode);
        virtual ~VKVertexShader();

        template <typename uniformType>
        void addBinding(VkFormat vkFormat, int noTuple);

        template <typename uniformType>
        void addInstanceBinding(VkFormat vkFormat, int noTuple);

        const VkVertexInputBindingDescription& getBindingDescription() const;
        
        const std::vector <VkVertexInputAttributeDescription>& 
            getAttributeDescriptions() const;

        VkVertexInputBindingDescription instanceBindingDescription;

    private:
        VkVertexInputBindingDescription bindingDescription;
        std::vector <VkVertexInputAttributeDescription> attributeDescriptions;

    };


    template <typename uniformType>
    void VKVertexShader::addBinding(VkFormat vkFormat, int noTuple) {
        VkVertexInputAttributeDescription attributeDescription { };

        attributeDescription.binding = 0;
        attributeDescription.location = (uint32_t)attributeDescriptions.size();
        attributeDescription.format = vkFormat;
        attributeDescription.offset = bindingDescription.stride;

        bindingDescription.stride += noTuple * sizeof(uniformType);

        attributeDescriptions.push_back(attributeDescription);

    }


    template <typename uniformType>
    void VKVertexShader::addInstanceBinding(VkFormat vkFormat, int noTuple) {
        VkVertexInputAttributeDescription attributeDescription { };

        attributeDescription.binding = 1;
        attributeDescription.location = (uint32_t)attributeDescriptions.size();
        attributeDescription.format = vkFormat;
        attributeDescription.offset = instanceBindingDescription.stride;

        instanceBindingDescription.stride += noTuple * sizeof(uniformType);

        attributeDescriptions.push_back(attributeDescription);

    }
}
