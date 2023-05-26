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

#include "opengl/GLTriangle.h"
#include "opengl/GLRectangle.h"
#include "opengl/GLShaders.h"
#include "opengl/GLTexture.h"
#include "opengl/GLMesh.h"

#include "util/UMaths.h"
#include "util/UFile.h"
#include "util/UColors.h"



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
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK) {
		std::cout << "Exit on glewInit()..." << std::endl;
	}

	std::cout << "Finished init." << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	//	main loop in while()

	aa::GLTexture* asteroidTex = aa::GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
	)	.setColorFile("Rock02_BaseColor", "tga")
		.build();

	aa::GLShader* meshShader = aa::GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader("v_3d_mesh")
		.setFragmentShader("f_3d_mesh")
		.addUniformTex("u_Texture", asteroidTex)
		.build();

	aa::GLMesh* obj = new aa::GLMesh(
		AA_ROOT,
		aa::Vector3d(300, 300, 0),
		meshShader
	);

	aa::Asteroid* ast = new aa::Asteroid(AA_ROOT,
		aa::Vector3d(0.0, 0.0f, 600.0f),
		obj
	);
	aa::SkyRectangle* sky = new aa::SkyRectangle(
		AA_ROOT
	);

	obj->loadFromFbx("D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX");
	sky->init();
	ast->init();

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
		ast->loop(lap);

		sky->draw();
		obj->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks

	delete asteroidTex;
	delete meshShader;
	delete sky;
	delete ast;
	delete obj;
	delete importer;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
#endif