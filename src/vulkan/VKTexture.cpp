#include "VKTexture.h"

#include <iostream>

//	3rd parties
#include "stb_image.h"

#include "VulkanRegistrar.h"

using namespace aa;



VKTexture::VKTexture() :
    textureImage        (VkImage{}),
    textureImageMemory  (VkDeviceMemory{})
{

}

VKTexture::~VKTexture()
{
    vkDestroyImage  (*VK_DEVICE, textureImage,       nullptr);
    vkFreeMemory    (*VK_DEVICE, textureImageMemory, nullptr);
}


void VKTexture::loadColormap(const char* filepath) 
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        filepath, 
        &texWidth, &texHeight, &texChannels, 
        STBI_rgb_alpha
    );
    if (pixels == 0) {
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
    memcpy       (*VK_DEVICE, pixels, (size_t)(imageSize));
    vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

    stbi_image_free(pixels);

    this->createImage(
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


void VKTexture::createImage(
    uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, 
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
    VkImage& image, VkDeviceMemory& imageMemory
) {
    VkImageCreateInfo imageInfo { };
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(*VK_DEVICE, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        std::cout << this << ": Failed to create image!\n";
    }

    VkMemoryRequirements memRequirements { };
    VkMemoryAllocateInfo allocInfo       { };
    vkGetImageMemoryRequirements(*VK_DEVICE, image, &memRequirements);

    allocInfo.sType             = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize    = memRequirements.size;
    allocInfo.memoryTypeIndex   = VulkanRegistrar::findDeviceMemoryType(
        memRequirements.memoryTypeBits, properties
    );

    if (vkAllocateMemory(*VK_DEVICE, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        std::cout << this << ": Failed to allocate image memory!";
    }

    vkBindImageMemory(*VK_DEVICE, image, imageMemory, 0);
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
