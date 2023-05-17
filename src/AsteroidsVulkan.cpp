//	GLFW for the game window
#include "vulkan/VulkanRegistrar.h"

#include "vulkan/VKShader.h"
#include "vulkan/VKPipeline.h"
#include "vulkan/VKTriangle.h"

//	cpp includes
#include <iostream>
#include <vector>

//	dev code
#include "window_constants.hpp"	// WINDOW_* macros

#include "shared/domain/RootObject.h"

#include "util/UFile.h"



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

	aa::VulkanRegistrar::registerVulkan(window);


	// create shader
	auto vertexBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-hardcoded_triangle.spv"
	);
	auto fragmentBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-hardcoded_triangle.spv"
	);
	auto vShader = new aa::VKShader(vertexBinaryContent);
	auto fShader = new aa::VKShader(fragmentBinaryContent);

	// create pipeline
	auto pipeline = aa::VKPipelineBuilder()
		.setVertexShader(vShader)
		.setFragmentShader(fShader)
		.build();

	auto tri = new aa::VKTriangle(
		AA_ROOT,
		pipeline
	);

	tri->init();

	//	main loop in while()

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();

		double currentTime = glfwGetTime();
		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;

		tri->draw();
		aa::VulkanRegistrar::loop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	vkDeviceWaitIdle(*VK_DEVICE);

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	delete tri;
	delete pipeline;
	delete vShader;
	delete fShader;

	aa::VulkanRegistrar::cleanVulkan();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif