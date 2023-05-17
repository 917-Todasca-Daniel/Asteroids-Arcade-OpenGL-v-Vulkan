#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define VK_INSTANCE           aa::VulkanRegistrar::getVkInstance()
#define VK_DEVICE             aa::VulkanRegistrar::getDevice()
#define VK_RENDER_PASS        aa::VulkanRegistrar::getRenderPass()
#define VK_COMMAND_BUFFER     aa::VulkanRegistrar::getCommandBuffer()
#define VK_SWAPCHAIN          aa::VulkanRegistrar::getSwapChain()

#define VK_GRAPHICS_QUEUE     aa::VulkanRegistrar::getGraphicsQueue()
#define VK_PRESENTATION_QUEUE aa::VulkanRegistrar::getPresentationQueue()

#define VK_FLIGHT_FENCE       aa::VulkanRegistrar::getInFlightFence()
#define VK_IMAGE_SEMAPHORE    aa::VulkanRegistrar::getImageSemaphore()
#define VK_RENDER_SEMAPHORE   aa::VulkanRegistrar::getRenderSemaphore()

#define VK_MAX_FRAMES_IN_FLIGHT 2


namespace aa
{
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

        // must call registerVulkan() before accessing
        VkRenderPass*    getRenderPass();

        VkCommandBuffer* getCommandBuffer();

        VkSwapchainKHR*  getSwapChain();

        VkQueue*         getGraphicsQueue();
        VkQueue*         getPresentationQueue();

        VkFence*         getInFlightFence();
        VkSemaphore*     getImageSemaphore();
        VkSemaphore*     getRenderSemaphore();

        void recordCommandBuffer(
            const VkCommandBuffer& buffer, 
            const VkPipeline&      graphicsPipeline,
            uint32_t               imageIndex
        );

        void recreateSwapChain();

        void loop();

    }
}
