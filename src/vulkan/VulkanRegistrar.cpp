#include "VKPipeline.h"
#include "VKShader.h"

#include <iostream>
#include <vector>
#include <set>
#include <optional>
#include <algorithm>

#include "constants/window_constants.hpp"

using namespace aa;


uint32_t currentFrame = 0;

VkInstance						_vkInstance;
VkDebugUtilsMessengerEXT		_debugMessenger;
VkQueue							_graphicsQueue;
VkQueue							_presentQueue;
VkSurfaceKHR					_drawSurface;
VkSwapchainKHR					_swapChain;
GLFWwindow*						_glfwWindow;
std::vector<VkImage>			_swapChainImages;
VkFormat						_swapChainImageFormat;
VkExtent2D						_swapChainExtent;
std::vector<VkImageView>		_swapChainImageViews;
VkRenderPass					_renderPass;
std::vector<VkFramebuffer>		_swapChainFramebuffers;
VkCommandPool					_commandPool;
std::vector<VkCommandBuffer>	_commandBuffers;
VkPhysicalDevice				_gpuWorker			= VK_NULL_HANDLE;
VkDevice						_logicWorker		= VK_NULL_HANDLE;
float							_queuePriorities	= 1.0f;
const char *					_engineName			= "Asteroidum";
const char *					_appName			= "Arcade Asteroids";
VkClearValue					_clearColor			= { {{0.0f, 0.0f, 0.0f, 1.0f}} };

std::vector<VkSemaphore>		_imageAvailableSemaphores;
std::vector<VkSemaphore>		_renderFinishedSemaphores;
std::vector<VkFence>			_inFlightFences;



const std::vector<const char*> _validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> _deviceExtensions = {
	"VK_KHR_swapchain",
	"VK_KHR_pipeline_library",
	"VK_EXT_graphics_pipeline_library"
};


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
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


VkSurfaceFormatKHR _chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& availableFormats
) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR _chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& availablePresentModes
) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D _chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR& capabilities
) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(_glfwWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width, capabilities.minImageExtent.width, 
			capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height, capabilities.minImageExtent.height, 
			capabilities.maxImageExtent.height
		);

		return actualExtent;
	}
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

	if (deviceProperties.limits.maxVertexInputBindings == 0) {
		return 0;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	// application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		score = 0;
	}

	std::cout << candidate << ' ' << score << '\n';

	return score;
}


SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	uint32_t presentModeCount;
	uint32_t formatCount;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		device, _drawSurface, &details.capabilities
	);
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device, _drawSurface, &formatCount, nullptr
	);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device, _drawSurface, &formatCount, details.formats.data()
		);
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device, _drawSurface, &presentModeCount, nullptr
	);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, _drawSurface, &presentModeCount, details.presentModes.data()
		);
	}

	return details;
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
	if (!_enableValidationLayers) return;

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

	std::cout << _gpuWorker << '\n';
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
	createInfo.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos	   = queueCreateInfos.data();
	createInfo.queueCreateInfoCount    = (uint32_t) (queueCreateInfos.size());
	createInfo.pEnabledFeatures		   = &deviceFeatures;
	createInfo.enabledExtensionCount   = (uint32_t) (_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

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
	_glfwWindow = window;
	if (glfwCreateWindowSurface(_vkInstance, _glfwWindow, nullptr, &_drawSurface) != VK_SUCCESS) {
		std::cout << "Failed to create window surface!";
	}
}

void _initSwapChain() {
	SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(_gpuWorker);

	VkSurfaceFormatKHR vkSurfaceFormat = _chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR   vkPresentMode   = _chooseSwapPresentMode  (swapChainSupport.presentModes);
	VkExtent2D		   vkExtent		   = _chooseSwapExtent	     (swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && 
		imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo { };
	createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface			= _drawSurface;
	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= vkSurfaceFormat.format;
	createInfo.imageColorSpace	= vkSurfaceFormat.colorSpace;
	createInfo.imageExtent		= vkExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	createInfo.preTransform		= swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode		= vkPresentMode;
	createInfo.clipped			= VK_TRUE;
	createInfo.oldSwapchain		= VK_NULL_HANDLE;

	QueueFamilyIndices indices = _findQueueFamilies(_gpuWorker);
	uint32_t queueFamilyIndices[] = { 
		indices.graphicsFamily.value(), indices.presentFamily.value() 
	};

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= 2;
		createInfo.pQueueFamilyIndices		= queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	}

	if (vkCreateSwapchainKHR(_logicWorker, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
		std::cout << "Failed to create swap chain!\n";
	}

	vkGetSwapchainImagesKHR(_logicWorker, _swapChain, &imageCount, nullptr);
	std::cout << "image count: " << imageCount << '\n';
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_logicWorker, _swapChain, &imageCount, _swapChainImages.data());

	_swapChainImageFormat = vkSurfaceFormat.format;
	_swapChainExtent = vkExtent;
}

void _initImageViews() {
	_swapChainImageViews.resize(_swapChainImages.size());

	for (size_t i = 0; i < _swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo { };
		createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image							= _swapChainImages[i];
		createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format							= _swapChainImageFormat;
		createInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel	= 0;
		createInfo.subresourceRange.levelCount		= 1;
		createInfo.subresourceRange.baseArrayLayer	= 0;
		createInfo.subresourceRange.layerCount		= 1;

		if (vkCreateImageView(
			_logicWorker, &createInfo, nullptr, &_swapChainImageViews[i]
		) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void _initRenderPass() {
	VkAttachmentDescription colorAttachment		{ };
	VkAttachmentReference	colorAttachmentRef	{ };
	VkSubpassDescription	subpass				{ };
	VkSubpassDependency		dependency			{ };
	VkRenderPassCreateInfo	renderPassInfo		{ };

	{
		colorAttachment.format			= _swapChainImageFormat;
		colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	{	// no additional subpasses (post-processing)
		colorAttachmentRef.attachment	= 0;
		colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	{	// main draw pass
		subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount	= 1;
		subpass.pColorAttachments		= &colorAttachmentRef;
	}

	{
		dependency.srcSubpass			= VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass			= 0;
		dependency.srcStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask		= 0;
		dependency.dstStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		renderPassInfo.dependencyCount	= 1;
		renderPassInfo.pDependencies	= &dependency;
	}

	{	// create the render pass
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= 1;
		renderPassInfo.pAttachments		= &colorAttachment;
		renderPassInfo.subpassCount		= 1;
		renderPassInfo.pSubpasses		= &subpass;
	}

	if (vkCreateRenderPass(_logicWorker, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
		std::cout << "Failed to create render pass!\n";
	}
}

void _initFrameBuffers() {
	_swapChainFramebuffers.resize(_swapChainImageViews.size());

	for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= _renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments	= attachments;
		framebufferInfo.width			= _swapChainExtent.width;
		framebufferInfo.height			= _swapChainExtent.height;
		framebufferInfo.layers			= 1;

		if (vkCreateFramebuffer(
			_logicWorker, 
			&framebufferInfo, 
			nullptr,
			&_swapChainFramebuffers[i]
		) != VK_SUCCESS) {
			std::cout << "Failed to create framebuffer!\n";
		}
	}
}

void _initCommandPool() {
	QueueFamilyIndices queueFamilyIndices = _findQueueFamilies(_gpuWorker);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex	= queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(_logicWorker, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
		std::cout << "Failed to create command pool!\n";
	}
}

void _initCommandBuffer() {
	_commandBuffers.resize(VK_MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType				 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool		 = _commandPool;
	allocInfo.level				 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

	if (vkAllocateCommandBuffers(
		_logicWorker, 
		&allocInfo,
		_commandBuffers.data()
	) != VK_SUCCESS) {
		std::cout << "Failed to allocate command buffers!\n";
	}
}

void _initSyncObjects() {
	_imageAvailableSemaphores.resize(VK_MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(VK_MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(VK_MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo	semaphoreInfo{ };
	VkFenceCreateInfo		fenceInfo	{ };

	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	fenceInfo.sType		= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags		= VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(_logicWorker, &semaphoreInfo, 
			nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(_logicWorker, &semaphoreInfo, 
				nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(_logicWorker, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {
			std::cout << "Failed to create synchronization objects for a frame!\n";
		}
	}
}


void VulkanRegistrar::registerVulkan(GLFWwindow* window) {
	_initVulkanInstance();
	_initDebug();
	_initWindow(window);
	_initPhysicalDevices();
	_initLogicalDevice();
	_initSwapChain();
	_initImageViews();
	_initRenderPass();
	_initFrameBuffers();
	_initCommandPool();
	_initCommandBuffer();
	_initSyncObjects();
}


void _cleanSwapChain() {
	for (auto framebuffer : _swapChainFramebuffers) {
		vkDestroyFramebuffer(_logicWorker, framebuffer, nullptr);
	}

	for (auto imageView : _swapChainImageViews) {
		vkDestroyImageView(_logicWorker, imageView, nullptr);
	}

	vkDestroySwapchainKHR(_logicWorker, _swapChain, nullptr);
}

void VulkanRegistrar::cleanVulkan() {
	_cleanSwapChain();

	for (size_t i = 0; i < VK_MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(_logicWorker, _imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(_logicWorker, _renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(_logicWorker, _inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(_logicWorker, _commandPool, nullptr);

	vkDestroyRenderPass(_logicWorker, _renderPass, nullptr);

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


VkInstance* VulkanRegistrar::getVkInstance() {
	return &_vkInstance;
}

VkDevice* aa::VulkanRegistrar::getDevice() {
	return &_logicWorker;
}

VkRenderPass* aa::VulkanRegistrar::getRenderPass() {
	return &_renderPass;
}

VkCommandBuffer* aa::VulkanRegistrar::getCommandBuffer() {
	return &_commandBuffers[currentFrame];
}

VkSwapchainKHR* aa::VulkanRegistrar::getSwapChain() {
	return &_swapChain;
}

VkQueue* aa::VulkanRegistrar::getGraphicsQueue() {
	return &_graphicsQueue;
}

VkQueue* aa::VulkanRegistrar::getPresentationQueue() {
	return &_presentQueue;
}

VkFence* aa::VulkanRegistrar::getInFlightFence() {
	return &_inFlightFences[currentFrame];
}

VkSemaphore* aa::VulkanRegistrar::getImageSemaphore() {
	return &_imageAvailableSemaphores[currentFrame];
}

VkSemaphore* aa::VulkanRegistrar::getRenderSemaphore() {
	return &_renderFinishedSemaphores[currentFrame];
}

uint32_t aa::VulkanRegistrar::getCurrentFrame() {
	return currentFrame;
}


void VulkanRegistrar::recordCommandBuffer(
	const VkCommandBuffer& buffer, 
    VKPipeline*            pipeline,
	uint32_t               imageIndex,
	VkBuffer*			   vertexBuffer
) {
	auto& graphicsPipeline = pipeline->vkPipeline;

	VkCommandBufferBeginInfo	beginInfo		{ };
	VkRenderPassBeginInfo		renderPassInfo	{ };
	VkViewport					viewport		{ };
	VkRect2D					scissor			{ };

	beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags				= 0;
	beginInfo.pInheritanceInfo	= nullptr; 

	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass		 = _renderPass;
	renderPassInfo.framebuffer		 = _swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = _swapChainExtent;
	renderPassInfo.clearValueCount	= 1;
	renderPassInfo.pClearValues		= &_clearColor;

	viewport.x			= 0.0f;
	viewport.y			= 0.0f;
	viewport.width		= (float)_swapChainExtent.width;
	viewport.height		= (float)_swapChainExtent.height;
	viewport.minDepth	= 0.0f;
	viewport.maxDepth	= 1.0f;

	scissor.offset		= { 0, 0 };
	scissor.extent		= _swapChainExtent;

	if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
		std::cout << "Failed to begin recording command buffer!\n";
	}

	vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline	(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdSetViewport	(buffer, 0, 1, &viewport);
	vkCmdSetScissor		(buffer, 0, 1, &scissor);

	if (vertexBuffer) {
		VkBuffer	 vertexBuffers[] = { *vertexBuffer };
		VkDeviceSize offsets[]		 = { 0 };

		auto& pipelineLayout = pipeline->vkPipelineLayout;
		VkDescriptorSet descriptorSet = pipeline->vertexShader->getDescriptorSet(currentFrame);

		vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindDescriptorSets(
			buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout, 0, 1,
			&descriptorSet,
			0, nullptr
		);
		vkCmdDraw(buffer, 3, 1, 0, 0);
	}
	else {
		vkCmdDraw(buffer, 3, 1, 0, 0);
	}

	vkCmdEndRenderPass	(buffer);

	if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
		std::cout << "Failed to record command buffer!\n";
	}
}

uint32_t aa::VulkanRegistrar::findDeviceMemoryType(
	uint32_t typeFilter, 
	VkMemoryPropertyFlags properties
) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_gpuWorker, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) 
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	std::cout << "Failed to find memory type!\n";
	return 0;
}


void VulkanRegistrar::createBuffer(
	VkDeviceSize size, 
	VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, 
	VkBuffer& buffer, 
	VkDeviceMemory& bufferMemory
) {
	VkBufferCreateInfo   bufferCreateInfo{ };
	VkMemoryRequirements memoryRequirements{ };
	VkMemoryAllocateInfo allocInfo{ };

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(*VK_DEVICE, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
		std::cout << "Failed to create vertex buffer!\n";
	}

	vkGetBufferMemoryRequirements(*VK_DEVICE, buffer, &memoryRequirements);

	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = VulkanRegistrar::findDeviceMemoryType(
		memoryRequirements.memoryTypeBits, properties
	);

	if (vkAllocateMemory(*VK_DEVICE, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		std::cout << "Failed to allocate vertex buffer memory!\n";
	}

	vkBindBufferMemory(*VK_DEVICE, buffer, bufferMemory, 0);
}


void aa::VulkanRegistrar::recreateSwapChain() {
	{	// minimizing
		int width = 0, height = 0;
		glfwGetFramebufferSize(_glfwWindow, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(_glfwWindow, &width, &height);
			glfwWaitEvents();
		}
	}

	vkDeviceWaitIdle(_logicWorker);
	
	_cleanSwapChain();

	_initSwapChain();
	_initImageViews();
	_initFrameBuffers();
}

void aa::VulkanRegistrar::loop() {
	currentFrame = (currentFrame + 1) % VK_MAX_FRAMES_IN_FLIGHT;
}
