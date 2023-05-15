#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define VK_INSTANCE aa::VulkanRegistrar::getVkInstance()

#define VK_DEVICE   aa::VulkanRegistrar::getDevice()


namespace aa
{
    // namespace contains methods dealing with everything about Vulkan setup
    namespace VulkanRegistrar {
        // creates vulkan instance, adds validation layers, registers devices
        void registerVulkan(GLFWwindow* window);

        // deallocates resources from aa::VulkanRegistrar::registerVulkan
        void cleanVulkan();

        // must call registerVulkan() before accessing
        const VkInstance& getVkInstance();

        // must call registerVulkan() before accessing
        const VkDevice& getDevice();

    }
}