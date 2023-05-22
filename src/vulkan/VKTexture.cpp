#include "VKTexture.h"

#include <iostream>

//	3rd parties
#include "stb_image.h"

#include "VulkanRegistrar.h"

using namespace aa;



VKTexture::VKTexture() :
    textureImageView    (VkImageView{}),
    textureImage        (VkImage{}),
    textureImageMemory  (VkDeviceMemory{}),
    textureSampler      (VkSampler{})
{

}

VKTexture::~VKTexture()
{
    vkDestroySampler    (*VK_DEVICE, textureSampler,     nullptr);
    vkDestroyImageView  (*VK_DEVICE, textureImageView,   nullptr);
    vkDestroyImage      (*VK_DEVICE, textureImage,       nullptr);
    vkFreeMemory        (*VK_DEVICE, textureImageMemory, nullptr);
}


void VKTexture::loadColormap(const char* filepath) 
{
    loadTextureImage(filepath);
    textureImageView = VulkanRegistrar::createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB);
    createTextureSampler();
}

void VKTexture::loadTextureImage(const char* filepath) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        filepath, 
        &texWidth, &texHeight, &texChannels, 
        STBI_rgb_alpha
    );
    if (!pixels) {
        std::cout << this << ": Failed to load texture image!\n";
        return;
    }

    VkBuffer        stagingBuffer;
    VkDeviceMemory  stagingBufferMemory;
    VkDeviceSize    imageSize = texWidth * texHeight * 4;
    void*           data;

    VulkanRegistrar::createBuffer(
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    vkMapMemory  (*VK_DEVICE, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy       (data, pixels, (size_t)(imageSize));
    vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

    stbi_image_free(pixels);

    VulkanRegistrar::createImage(
        texWidth, texHeight, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        textureImage, 
        textureImageMemory
    );

    transitionImageLayout(
        textureImage, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
    copyBufferToImage(stagingBuffer, textureImage, (uint32_t)texWidth, (uint32_t)texHeight);
    transitionImageLayout(
        textureImage, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    vkDestroyBuffer (*VK_DEVICE, stagingBuffer,       nullptr);
    vkFreeMemory    (*VK_DEVICE, stagingBufferMemory, nullptr);

}

void VKTexture::createTextureSampler() {
    VkPhysicalDeviceProperties  properties  { };
    VkSamplerCreateInfo         samplerInfo { };

    vkGetPhysicalDeviceProperties(*VK_PHYSICAL_DEVICE, &properties);

    samplerInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter           = VK_FILTER_LINEAR;
    samplerInfo.minFilter           = VK_FILTER_LINEAR;
    samplerInfo.addressModeU        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable    = VK_TRUE;
    samplerInfo.maxAnisotropy       = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor         = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable       = VK_FALSE;
    samplerInfo.compareOp           = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode          = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias          = 0.0f;
    samplerInfo.minLod              = 0.0f;
    samplerInfo.maxLod              = 0.0f;

    if (vkCreateSampler(*VK_DEVICE, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        std::cout << this << ": Failed to create texture sampler!\n";
    }
}


void VKTexture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = VulkanRegistrar::preCommand();

    VkBufferImageCopy region{};
    region.bufferOffset                 = 0;
    region.bufferRowLength              = 0;
    region.bufferImageHeight            = 0;
    region.imageSubresource.aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel    = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount  = 1;
    
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
        commandBuffer, buffer, image, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        1, 
        &region
    );

    VulkanRegistrar::postCommand(commandBuffer);
}


void VKTexture::transitionImageLayout(
    VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout
) {
    VkCommandBuffer commandBuffer = VulkanRegistrar::preCommand();

    VkImageMemoryBarrier barrier          { };
    VkPipelineStageFlags sourceStage      { };
    VkPipelineStageFlags destinationStage { };

    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
        && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        std::cout << "Unsupported layout transition!\n";
    }

    vkCmdPipelineBarrier(
        commandBuffer, sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VulkanRegistrar::postCommand(commandBuffer);
}
