#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define VK_INSTANCE           aa::VulkanRegistrar::getVkInstance()
#define VK_DEVICE             aa::VulkanRegistrar::getDevice()
#define VK_PHYSICAL_DEVICE    aa::VulkanRegistrar::getPhysicalDevice()
#define VK_RENDER_PASS        aa::VulkanRegistrar::getRenderPass()
#define VK_COMMAND_BUFFER     aa::VulkanRegistrar::getCommandBuffer()
#define VK_SWAPCHAIN          aa::VulkanRegistrar::getSwapChain()

#define VK_GRAPHICS_QUEUE     aa::VulkanRegistrar::getGraphicsQueue()
#define VK_PRESENTATION_QUEUE aa::VulkanRegistrar::getPresentationQueue()

#define VK_FLIGHT_FENCE       aa::VulkanRegistrar::getInFlightFence()
#define VK_IMAGE_SEMAPHORE    aa::VulkanRegistrar::getImageSemaphore()
#define VK_RENDER_SEMAPHORE   aa::VulkanRegistrar::getRenderSemaphore()

#define VK_CURRENT_FRAME      aa::VulkanRegistrar::getCurrentFrame()

#define VK_CURRENT_IMAGE      aa::VulkanRegistrar::getImageIndex()

#define VK_MAX_FRAMES_IN_FLIGHT 2


namespace aa
{

    class VKPipeline;


    // namespace contains methods dealing with everything about Vulkan setup
    namespace VulkanRegistrar {
        // creates vulkan instance, adds validation layers, registers devices
        void registerVulkan(GLFWwindow* window);

        // deallocates resources from aa::VulkanRegistrar::registerVulkan
        void cleanVulkan();

        // must call registerVulkan() before accessing
        VkInstance*      getVkInstance();

        // must call registerVulkan() before accessing
        VkDevice*        getDevice();

        VkPhysicalDevice* getPhysicalDevice();

        // must call registerVulkan() before accessing
        VkRenderPass*    getRenderPass();

        VkCommandBuffer* getCommandBuffer();

        VkSwapchainKHR*  getSwapChain();

        VkQueue*         getGraphicsQueue();
        VkQueue*         getPresentationQueue();

        VkFence*         getInFlightFence();
        VkSemaphore*     getImageSemaphore();
        VkSemaphore*     getRenderSemaphore();

        uint32_t         getCurrentFrame();

        uint32_t         getImageIndex();

        void recordCommandBuffer(
            const VkCommandBuffer& buffer, 
            VKPipeline*            pipeline,
            uint32_t               imageIndex,
            VkBuffer*              vertexBuffer = nullptr,
            VkBuffer*              indexBuffer  = nullptr,
            uint32_t               indexNo = 0,
            uint32_t               instanceCount = 1
        );

        uint32_t findDeviceMemoryType(uint32_t, VkMemoryPropertyFlags);

        void recreateSwapChain();

        void createBuffer(
            VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags,
            VkBuffer&, VkDeviceMemory&
        );

        VkImageView createImageView(const VkImage &image, const VkFormat &format, VkImageAspectFlags flag = VK_IMAGE_ASPECT_COLOR_BIT);

        void createImage(
            uint32_t, uint32_t, VkFormat, VkImageTiling,
            VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&, VkDeviceMemory&
        );

        VkCommandBuffer preCommand();
        void postCommand(const VkCommandBuffer&);

        // issues a copy command
        void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);

        void loop();

        void predraw();

        void postdraw();

    }
}
