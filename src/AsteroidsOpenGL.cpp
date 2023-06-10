//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	assimp
#include <assimp/Importer.hpp>

//	cpp includes
#include <iostream>

//	dev code
#include "window_constants.hpp"	// WINDOW_* macros

#include "shared/domain/RootObject.h"
#include "shared/domain/Asteroid.h"
#include "shared/domain/SkyRectangle.h"
#include "shared/domain/GameFactory.h"

#include "util/UMaths.h"
#include "util/UFile.h"
#include "util/UColors.h"
#include "util/UInput.h"



#ifdef IMPL_OPENGL
int main() {
	std::cout << "Hello OpenGL!" << std::endl;


	//	init tasks
	
	Assimp::Importer* importer = new Assimp::Importer();
	aa::UFile::setAssimpFileReader(importer);

	GLFWwindow* window;

	if (!glfwInit()) {
		std::cout << "Exit on glfwInit()..." << std::endl;
		return -1;
	}

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

	if (glewInit() != GLEW_OK) {
		std::cout << "Exit on glewInit()..." << std::endl;
	}

	std::cout << "Finished init." << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	//	main loop in while()

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

	auto sky = new aa::SkyRectangle(AA_ROOT);
	sky->init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int noFrames = 0;
	double prevFrameTimestamp = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		double currentTime = glfwGetTime();

		noFrames++;
		if (currentTime - prevFrameTimestamp >= 1.0) {
			std::cout << noFrames / (currentTime - prevFrameTimestamp) << " FPS\n";
			noFrames = 0;
			prevFrameTimestamp = currentTime;
		}

		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;

		sky->loop(lap);
		for (auto& ast : asteroids) {
			ast->loop(lap);
		}

		sky->draw();
		for (auto& ast : asteroids) {
			ast->draw();
		}

		FACTORY->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	delete sky;
	for (auto& ast : asteroids) {
		delete ast;
	}
	delete importer;

	delete FACTORY;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif