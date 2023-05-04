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

#include "opengl/GLTriangle.h"
#include "opengl/GLRectangle.h"
#include "opengl/GLShaders.h"
#include "opengl/GLTexture.h"
#include "opengl/GLMesh.h"

#include "util/UMaths.h"
#include "util/UFile.h"
#include "util/UColors.h"


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
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "Exit on glewInit()..." << std::endl;
	}

	std::cout << "Finished init." << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	//	main loop in while()

	aa::GLTexture* tex = aa::GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\ambientcg"
	)	.setColorFile("Ground063_1K_Color", "jpg")
		.build();

	aa::GLTexture* asteroidTex = aa::GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
	)	.setColorFile("Rock02_BaseColor", "tga")
		.build();
	
	aa::GLShader* flatShader = aa::GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader("v_basic_position")
		.setFragmentShader("f_uniform_color")
		.addUniform4f("u_Color", aa::UColors::PINK)
		.build();

	aa::GLShader* texShader = aa::GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader("v_basic_tex_position")
		.setFragmentShader("f_basic_tex")
		.addUniformTex("u_Texture", tex)
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
	obj->loadFromFbx("D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX");

	aa::Asteroid* ast = new aa::Asteroid(AA_ROOT,
		aa::Vector3d(0.0, 0.0f, 0.0f),
		obj
	);
	ast->init();


	float ax, ay, bx, by, cx, cy;
	ax = WINDOW_WIDTH / 2;
	ay = 380;
	float alt = 125;
	aa::UMaths::worldTriangleTopAltitude(alt, ax, ay, bx, by, cx, cy);
	aa::GLTriangle* tri1 = new aa::GLTriangle(
		AA_ROOT,
		aa::Vector3d(ax, ay, 0),
		alt,
		flatShader
	);
	aa::GLTriangle* tri2 = new aa::GLTriangle(
		AA_ROOT,
		aa::Vector3d(bx, by, 0),
		alt,
		flatShader
	);
	aa::GLTriangle* tri3 = new aa::GLTriangle(
		AA_ROOT,
		aa::Vector3d(cx, cy, 0),
		alt,
		flatShader
	);
	tri1->init();
	tri2->init();
	tri3->init();
	aa::GLRectangle* rect1 = new aa::GLRectangle(
		AA_ROOT,
		aa::Vector3d(20, WINDOW_HEIGHT * 0.75f, 0),
		WINDOW_HEIGHT * 0.5f, 80,
		flatShader
	);
	rect1->init();

	aa::GLRectangle* rect2 = new aa::GLRectangle(
		AA_ROOT,
		aa::Vector3d(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 0),
		WINDOW_HEIGHT, WINDOW_WIDTH,
		texShader
	);
	rect2->init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window)) {
		static double previousTime = glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentTime = glfwGetTime();
		float lap = (float)(currentTime - previousTime);
		previousTime = currentTime;
		
		ast->loop(lap);
		rect2->draw();

		obj->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Finished main loop." << std::endl;


	//	cleanup tasks
	
	delete tri1;
	delete tri2;
	delete tri3;
	delete rect1;
	delete rect2;
	delete meshShader;
	delete obj;
	delete flatShader;
	delete texShader;
	delete tex;
	delete importer;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}