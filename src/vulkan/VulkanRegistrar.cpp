#include "VulkanRegistrar.h"

#include <iostream>
#include <vector>

#include "constants/window_constants.hpp"

using namespace aa;


VkInstance _vkInstance;

VkDebugUtilsMessengerEXT _debugMessenger;

const std::vector<const char*> _validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


#ifdef NDEBUG
	const bool _enableValidationLayers = true;
#else
	const bool _enableValidationLayers = true;
#endif


// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : _validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> getMandatoryVulkanExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (_enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


void initVulkanInstance() {
	if (_enableValidationLayers) {
		std::cout << "Validation layers will be loaded...\n";
	}
	if (_enableValidationLayers && !checkValidationLayerSupport()) {
		std::cout << "Selected validation layers not supported!\n";
	}

	VkApplicationInfo vkAppInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,	// sType
		nullptr,							// pNext
		"Vulkan Asteroids",					// pApplicationName
		VK_MAKE_VERSION(1, 0, 0),			// applicationVersion
		"Asteroidum",						// pEngineName
		VK_MAKE_VERSION(1, 0, 0),			// engineVersion
		VK_API_VERSION_1_3					// apiVersion
	};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector <const char*> vkExtensions = getMandatoryVulkanExtensions();
	VkInstanceCreateInfo createInfo{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	  // sType
		nullptr,								  // pNext
		0,										  // flags
		&vkAppInfo,								  // pApplicationInfo
		(_enableValidationLayers ? 
			(uint32_t) (_validationLayers.size())  
			: 0),								  // enabledLayerCount
		(_enableValidationLayers ?
			_validationLayers.data()
			: nullptr),			    			  // ppEnabledLayerName
		(uint32_t) (vkExtensions.size()),		  // enabledExtensionCount
		vkExtensions.data()						  // ppEnabledExtensionNames
	};

	if (vkCreateInstance(&createInfo, nullptr, &_vkInstance) != VK_SUCCESS) {
		std::cout << "Failed to create Vulkan App instance!\n";
	}
}

void initDebug() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,	// sType
		nullptr,													// pNext
		0,															// flags
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,				// messageSeverity
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,			// messageType
		_debugCallback,												// pfnUserCallback
		nullptr,													// pUserData
	};

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(_vkInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		if (func(_vkInstance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
			std::cout << "Failed to setup Vulkan debug messenger\n";
		}
	}
	else {
		std::cout << "Failed to setup Vulkan debug messenger - nullptr func\n";
	}
}


void VulkanRegistrar::registerVulkan() {
	initVulkanInstance();
	initDebug();
}

void VulkanRegistrar::cleanVulkan() {
	if (_enableValidationLayers) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(_vkInstance, _debugMessenger, nullptr);
		}
	}

	vkDestroyInstance(_vkInstance, nullptr);
}


const VkInstance& VulkanRegistrar::getVkInstance() {
	return _vkInstance;
}
