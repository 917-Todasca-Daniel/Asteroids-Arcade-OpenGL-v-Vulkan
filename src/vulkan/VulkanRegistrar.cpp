#include "VulkanRegistrar.h"

#include <iostream>
#include <vector>
#include <set>
#include <optional>

#include "constants/window_constants.hpp"

using namespace aa;


VkInstance				 _vkInstance;
VkDebugUtilsMessengerEXT _debugMessenger;
VkQueue					 _graphicsQueue;
VkQueue					 _presentQueue;
VkPhysicalDevice		 _gpuWorker		 = VK_NULL_HANDLE;
VkDevice				 _logicWorker	 = VK_NULL_HANDLE;
VkSurfaceKHR			 _drawSurface;
float					 _queuePriorities = 1.0f;
const char *			 _engineName	 = "Asteroidum";
const char *			 _appName		 = "Arcade Asteroids";

const std::vector<const char*> _validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


#ifdef NDEBUG
	const bool _enableValidationLayers = true;
#else
	const bool _enableValidationLayers = true;
#endif


// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _drawSurface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}


// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
bool _checkValidationLayerSupport() {
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

std::vector<const char*> _getMandatoryVulkanExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	// includes VK_KHR_surface
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (_enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


int _rateDevice(VkPhysicalDevice candidate) {
	VkPhysicalDeviceProperties	deviceProperties;
	VkPhysicalDeviceFeatures	deviceFeatures;
	int score = 0;

	vkGetPhysicalDeviceProperties	(candidate, &deviceProperties);
	vkGetPhysicalDeviceFeatures		(candidate, &deviceFeatures);

	// discrete GPUs are preffered
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	// application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		score = 0;
	}

	std::cout << candidate << ' ' << score << '\n';

	return score;
}


VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT				messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


void _initVulkanInstance() {
	if (_enableValidationLayers) {
		std::cout << "Validation layers will be loaded...\n";
	}
	if (_enableValidationLayers && !_checkValidationLayerSupport()) {
		std::cout << "Selected validation layers not supported!\n";
	}

	VkApplicationInfo vkAppInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,	// sType
		nullptr,							// pNext
		_appName,							// pApplicationName
		VK_MAKE_VERSION(1, 0, 0),			// applicationVersion
		_engineName,						// pEngineName
		VK_MAKE_VERSION(1, 0, 0),			// engineVersion
		VK_API_VERSION_1_3					// apiVersion
	};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector <const char*> vkExtensions = _getMandatoryVulkanExtensions();
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

void _initDebug() {
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

void _initPhysicalDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		std::cout << "No GPUs with Vulkan support! Project is canceled.\n";
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, devices.data());

	std::multiset <std::pair <int, VkPhysicalDevice>> scoredDevices;
	for (const auto& device : devices) {
		scoredDevices.insert(
			std::pair <int, VkPhysicalDevice>(
				_rateDevice(device),
				device
			)
		);
	}

	if (scoredDevices.rbegin()->first > 0) {
		_gpuWorker = scoredDevices.rbegin()->second;
	}

	if (_gpuWorker == VK_NULL_HANDLE) {
		std::cout << "Failed to find a suitable GPU for Vulkan!";
	}
}

void _initLogicalDevice() {
	QueueFamilyIndices indices = _findQueueFamilies(_gpuWorker);


	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { 
		indices.graphicsFamily.value(), 
		indices.presentFamily.value() 
	};

	for (uint32_t queueFamilyIndex : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,	// sType
			nullptr,									// pNext
			0,											// flags
			queueFamilyIndex,							// queueFamilyIndex
			1,											// queueCount
			&_queuePriorities,							// pQueuePriorities
		};
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures { };

	VkDeviceCreateInfo createInfo { };
	createInfo.sType				 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos	 = queueCreateInfos.data();
	createInfo.queueCreateInfoCount  = (uint32_t) (queueCreateInfos.size());
	createInfo.pEnabledFeatures		 = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	if (_enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
		createInfo.ppEnabledLayerNames = _validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_gpuWorker, &createInfo, nullptr, &_logicWorker) != VK_SUCCESS) {
		std::cout << "Failed to create logical device!\n";
	}

	vkGetDeviceQueue(_logicWorker, indices.graphicsFamily.value(), 0, &_graphicsQueue);
	vkGetDeviceQueue(_logicWorker, indices.presentFamily .value(), 0, &_presentQueue);
}

void _initWindow(GLFWwindow *window) {
	if (glfwCreateWindowSurface(_vkInstance, window, nullptr, &_drawSurface) != VK_SUCCESS) {
		std::cout << "Failed to create window surface!";
	}
}

void VulkanRegistrar::registerVulkan(GLFWwindow* window) {
	_initVulkanInstance();
	_initDebug();
	_initWindow(window);
	_initPhysicalDevices();
	_initLogicalDevice();
}

void VulkanRegistrar::cleanVulkan() {
	vkDestroyDevice(_logicWorker, nullptr);

	if (_enableValidationLayers) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(_vkInstance, _debugMessenger, nullptr);
		}
	}

	vkDestroySurfaceKHR(_vkInstance, _drawSurface, nullptr);

	vkDestroyInstance(_vkInstance, nullptr);
}


const VkInstance& VulkanRegistrar::getVkInstance() {
	return _vkInstance;
}
