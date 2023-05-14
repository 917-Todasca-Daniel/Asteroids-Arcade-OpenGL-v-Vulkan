//	GLFW for the game window
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//	cpp includes
#include <iostream>
#include <vector>

//	dev code
#include "window_constants.hpp"	// WINDOW_* macros



#ifdef IMPL_VULKAN
int main() {
	std::cout << "Knock knock Vulkan!" << std::endl;


	//	init tasks
	
	GLFWwindow* window;

	if (!glfwInit()) {
		std::cout << "Exit on glfwInit()..." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(
		WINDOW_WIDTH_SM, 
		WINDOW_HEIGHT_SM, 
		WINDOW_GAME_TITLE, 
		NULL, 
		NULL
	);

	if (!window)
	{
		std::cout << "Exit on glfwCreateWindow()..." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> vkExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensions.data());

	for (const auto& extension : vkExtensions) {
		std::cout << '\t' << extension.extensionName << '\n';
	}

	std::cout << extensionCount << " VK extensions supported\n";

	VkInstance vkInstance;

	{	// setting data for vulkan instance
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

		VkInstanceCreateInfo createInfo{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			&vkAppInfo,								// pApplicationInfo
			0,										// enabledLayerCount
			nullptr,								// ppEnabledLayerName
			glfwExtensionCount,						// enabledExtensionCount
			glfwExtensions							// ppEnabledExtensionNames
		};

		if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
			std::cout << "Failed to create Vulkan App instance!\n";
		}
	}


	//	main loop in while()

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();

		double currentTime = glfwGetTime();
		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	vkDestroyInstance(vkInstance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif