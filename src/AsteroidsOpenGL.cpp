//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	cpp includes
#include <iostream>

//	dev code
#include "window_constants.hpp"	// WINDOW_* macros

#include "shared/domain/RootObject.h"
#include "opengl/GLTriangle.h"


int main() {
	std::cout << "Hello OpenGL!" << std::endl;


	//	init tasks
	
	GLFWwindow* window;

	if (!glfwInit()) {
		std::cout << "Exit on glfwInit()..." << std::endl;
		return -1;
	}

	window = glfwCreateWindow(WINDOW_WIDTH_SM, WINDOW_HEIGHT_SM, WINDOW_GAME_TITLE, NULL, NULL);
	if (!window)
	{
		std::cout << "Exit on glfwCreateWindow()..." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "Exit on glewInit()..." << std::endl;
	}

	std::cout << "Finished init." << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	//	main loop in while()
	
	aa::GLTriangle* tri = new aa::GLTriangle(AA_ROOT);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		tri->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks
	
	delete tri;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}