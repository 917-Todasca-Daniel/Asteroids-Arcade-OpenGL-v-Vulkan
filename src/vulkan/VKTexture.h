#pragma once

#include "domain/Object3d.h"

#include "VulkanRegistrar.h"


namespace aa
{


    // wrapper over Vulkan textures
    // use ponter references since consumed memory gets large
    class VKTexture
    {

    public:
        VKTexture();
        virtual ~VKTexture();

        void loadColormap(const char* filepath);

        //  delete all implicit constructors 
        VKTexture(const VKTexture&) = delete;
        VKTexture(VKTexture&&) = delete;

        VKTexture& operator = (const VKTexture&) = delete;
        VKTexture& operator = (VKTexture&&) = delete;

        const VkImageView& getTextureImageView() const {
            return textureImageView;
        }

        const VkSampler& getTextureSampler() const {
            return textureSampler;
        }


    private:
        VkImage         textureImage;
        VkDeviceMemory  textureImageMemory;
        VkImageView     textureImageView;
        VkSampler       textureSampler;

        void loadTextureImage(const char* filepath);
        void createTextureSampler();

        void transitionImageLayout  (VkImage,  VkFormat, VkImageLayout, VkImageLayout);
        void copyBufferToImage      (VkBuffer, VkImage,  uint32_t,      uint32_t);

    };
}
