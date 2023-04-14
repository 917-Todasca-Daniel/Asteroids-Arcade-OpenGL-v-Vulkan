//	OpenGL related includes
#include <GLFW/glfw3.h>

//	cpp includes
#include <iostream>

//	dev code
#include "shared/window_constants.hpp"	// WINDOW_* macros


int main() {
	std::cout << "Hello OpenGL!\n";

	//	init tasks
	GLFWwindow* window;

	if (!glfwInit()) {
		std::cout << "Exit on glfwInit()...";
		return -1;
	}

	window = glfwCreateWindow(WINDOW_WIDTH_SM, WINDOW_HEIGHT_SM, WINDOW_GAME_TITLE, NULL, NULL);
	if (!window)
	{
		std::cout << "Exit on glfwCreateWindow()...";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	std::cout << "Finished init.";

	//	main loop in while()
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	std::cout << "Finished main loop.";

	//	cleanup tasks
	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}