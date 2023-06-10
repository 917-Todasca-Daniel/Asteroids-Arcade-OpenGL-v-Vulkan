//	GLFW for the game window
#include "vulkan/VulkanRegistrar.h"

#include "shared/domain/GameFactory.h"
#include "shared/domain/SkyRectangle.h"

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
#include "util/UInput.h"



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
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
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
	glfwSwapInterval(0);

	glfwSetKeyCallback(window, aa::UInput::glfwKeyCallback);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> vkExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensions.data());

	for (const auto& extension : vkExtensions) {
		std::cout << '\t' << extension.extensionName << '\n';
	}

	std::cout << extensionCount << " VK extensions supported\n";

	aa::VulkanRegistrar::registerVulkan(window);

	auto ship = SHIP_FACTORY->buildSpaceship();
	ship->init();

	auto sky = new aa::SkyRectangle(AA_ROOT);
	sky->init();

	std::vector <aa::Object3d*> asteroids;
	for (int i = 0; i < NUM_ASTEROIDS; i++) {
		auto ast = FACTORY->buildLargeAsteroid();
		ast->init();
		asteroids.push_back(ast);
	}
	for (int i = 0; i < NUM_ASTEROIDS; i++) {
		auto ast = FACTORY->buildSmallAsteroid();
		ast->init();
		asteroids.push_back(ast);
	}

	//	main loop in while()

	int noFrames = 0;
	double prevFrameTimestamp = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();
		double currentTime = glfwGetTime();

		noFrames++;
		if (currentTime - prevFrameTimestamp >= 1.0) {
			std::cout << noFrames / (currentTime - prevFrameTimestamp) << " FPS\n";
			noFrames = 0;
			prevFrameTimestamp = currentTime;
		}

		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;

		{	// loop objects
			for (auto& ast : asteroids) {
				ast->loop(lap);
			}
			sky->loop(lap);

			ship->loop(lap);
		}

		{	// draw in vulkan
			aa::VulkanRegistrar::predraw();

			sky->draw();
			for (auto& ast : asteroids) {
				ast->draw();
			}

			ship->draw();
			FACTORY->draw();

			aa::VulkanRegistrar::postdraw();
		}

		aa::VulkanRegistrar::loop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	vkDeviceWaitIdle(*VK_DEVICE);

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	delete ship;
	for (auto& ast : asteroids) {
		delete ast;
	}
	delete sky;
	delete importer;

	delete FACTORY;

	aa::VulkanRegistrar::cleanVulkan();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif