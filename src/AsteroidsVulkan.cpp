//	GLFW for the game window
#include "vulkan/VulkanRegistrar.h"

#include "vulkan/VKShader.h"
#include "vulkan/VKPipeline.h"
#include "vulkan/VKTriangle.h"
#include "vulkan/VKRectangle.h"
#include "vulkan/VKMesh.h"

//	cpp includes
#include <iostream>
#include <vector>

//	assimp
#include <assimp/Importer.hpp>

//	dev code
#include "window_constants.hpp"	// WINDOW_* macros

#include "shared/domain/RootObject.h"
#include "shared/domain/Asteroid.h"

#include "util/UFile.h"



#ifdef IMPL_VULKAN
int main() {
	std::cout << "Knock knock Vulkan!" << std::endl;


	//	init tasks

	Assimp::Importer* importer = new Assimp::Importer();
	aa::UFile::setAssimpFileReader(importer);
	
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
	auto skyVertexBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-perlin_sky.spv"
	);
	auto skyFragmentBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-perlin_sky.spv"
	);

	auto skyVShader = new aa::VKVertexShader(skyVertexBinaryContent);
	skyVShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT, 2);
	skyVShader->addUniform(sizeof(float));
	auto skyFShader = new aa::VKShader(skyFragmentBinaryContent);

	auto asteroidVertexBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-3d_mesh.spv"
	);
	auto asteroidFragmentBinaryContent = aa::UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-3d_mesh.spv"
	);

	auto astVShader = new aa::VKVertexShader(asteroidVertexBinaryContent);
	astVShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	astVShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	astVShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT, 2);
	astVShader->addUniform(16 * sizeof(float));
	auto astFShader = new aa::VKShader(asteroidFragmentBinaryContent);

	// create pipeline
	auto skyPipeline = aa::VKPipelineBuilder()
		.setVertexShader(skyVShader)
		.setFragmentShader(skyFShader)
		.build();

	auto astPipeline = aa::VKPipelineBuilder()
		.setVertexShader(astVShader)
		.setFragmentShader(astFShader)
		.build();

	auto rect = new aa::VKRectangle(
		AA_ROOT,
		aa::Vector3d(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 0),
		WINDOW_HEIGHT, WINDOW_WIDTH,
		skyPipeline
	);

	auto ast = new aa::VKMesh(
		AA_ROOT,
		aa::Vector3d(0.0f, 0.0f, 0.0f),
		astPipeline
	);

	ast->loadFromFbx("D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX");

	rect->init();
	ast->init();

	//	main loop in while()

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();

		double currentTime = glfwGetTime();
		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;

		{
			aa::VulkanRegistrar::predraw();

			ast->loop(lap);
			rect->loop(lap);
			rect->draw();
			ast->draw();

			aa::VulkanRegistrar::postdraw();
		}

		aa::VulkanRegistrar::loop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	vkDeviceWaitIdle(*VK_DEVICE);

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	delete ast;
	delete astPipeline;
	delete astFShader;
	delete astVShader;

	delete rect;
	delete skyPipeline;
	delete skyVShader;
	delete skyFShader;

	aa::VulkanRegistrar::cleanVulkan();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif