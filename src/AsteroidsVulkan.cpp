//	GLFW for the game window
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//	cpp includes
#include <iostream>

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

	std::cout << extensionCount << " VK extensions supported\n";


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

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif