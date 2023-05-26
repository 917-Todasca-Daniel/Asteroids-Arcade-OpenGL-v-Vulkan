#include "GameFactory.h"

// vulkan includes
#include "../vulkan/VulkanRegistrar.h"

#include "../vulkan/VKShader.h"
#include "../vulkan/VKPipeline.h"
#include "../vulkan/VKTriangle.h"
#include "../vulkan/VKRectangle.h"
#include "../vulkan/VKMesh.h"
#include "../vulkan/VKTexture.h"

// opengl includes
#include "../opengl/GLTriangle.h"
#include "../opengl/GLRectangle.h"
#include "../opengl/GLShaders.h"
#include "../opengl/GLTexture.h"
#include "../opengl/GLMesh.h"

// dev includes
#include "domain/RootObject.h"
#include "domain/Asteroid.h"

#include "util/UFile.h"

#include "data/Vector3d.h"

#include "window_constants.hpp"

using namespace aa;


#ifdef IMPL_VULKAN
	GameFactory* GameFactory::instance = new VulkanGraphicsFactory();
#endif
#ifdef IMPL_OPENGL
	GameFactory* GameFactory::instance = new OpenGLGraphicsFactory();
#endif


GameFactory::GameFactory()
{

}

GameFactory::~GameFactory()
{

}


GameFactory* GameFactory::getFactory() {
	return instance;
}



OpenGLGraphicsFactory::OpenGLGraphicsFactory() : asteroidTex(nullptr)
{

}

OpenGLGraphicsFactory::~OpenGLGraphicsFactory()
{
	for (auto& shader : shaders) {
		delete shader;
	}

	if (asteroidTex) delete asteroidTex;

}


Object3d* OpenGLGraphicsFactory::buildSky(float height, float width, float* time) {
	auto shader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader	("v_perlin_sky")
		.setFragmentShader	("f_perlin_sky")
		.build();

	auto rectangle = new GLRectangle(AA_ROOT, Vector3d(width*0.5f, height*0.5f, 0), height, width, shader);

	shader->addUniform1iRef("u_Time", time);
	shaders.push_back(shader);

	return rectangle;
}

Object3d* OpenGLGraphicsFactory::buildLargeAsteroid() {
	if (!asteroidTex) {
		asteroidTex = aa::GLTextureFileBuilder(
			"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
		).setColorFile("Rock02_BaseColor", "tga")
			.build();
	}

	aa::GLShader* meshShader = aa::GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader	("v_3d_mesh")
		.setFragmentShader	("f_3d_mesh")
		.addUniformTex		("u_Texture", asteroidTex)
		.build();

	shaders.push_back(meshShader);

	aa::GLMesh*  obj  = new aa::GLMesh  (AA_ROOT, aa::Vector3d(0, 0, 0),	     meshShader);
	aa::Asteroid* ast = new aa::Asteroid(AA_ROOT, aa::Vector3d(0.0, 0.0f, 0.0f), obj);

	obj->loadFromFbx("D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX");

	return ast;
}



VulkanGraphicsFactory::VulkanGraphicsFactory()
	: tex(nullptr), skyVertexShader(nullptr), meshVertexShader(nullptr),
	skyFragmentShader(nullptr), meshFragmentShader(nullptr),
	skyPipeline(nullptr), meshPipeline(nullptr)
{

}

VulkanGraphicsFactory::~VulkanGraphicsFactory()
{
	if (tex)				delete tex;
	if (skyVertexShader)	delete skyVertexShader;
	if (meshVertexShader)	delete meshVertexShader;
	if (skyFragmentShader)	delete skyFragmentShader;
	if (meshFragmentShader) delete meshFragmentShader;
	if (skyPipeline)		delete skyPipeline;
	if (meshPipeline)		delete meshPipeline;

}


Object3d* VulkanGraphicsFactory::buildSky(float height, float width, float* time) {
	if (skyPipeline == nullptr) {
		buildSkyPrereq();
	}

	return new VKRectangle(AA_ROOT, Vector3d(width*0.5f, height*0.5f, 0), height, width, skyPipeline);
}

Object3d* VulkanGraphicsFactory::buildLargeAsteroid() {
	if (meshPipeline == nullptr) {
		buildMeshPrereq();
	}

	VKMesh*   mesh		= new VKMesh  (AA_ROOT, aa::Vector3d(0, 0, 0.0f), meshPipeline);
	Asteroid* asteroid  = new Asteroid(AA_ROOT, aa::Vector3d(0, 0, 0),    mesh);

	mesh->loadFromFbx("D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX");

	return asteroid;
}


void VulkanGraphicsFactory::buildSkyPrereq() {
	auto skyVertexBinaryContent		= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-perlin_sky.spv"
	);
	auto skyFragmentBinaryContent	= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-perlin_sky.spv"
	);

	{	// bind vertex shader data
		skyVertexShader = new VKVertexShader(skyVertexBinaryContent);
		skyVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
		skyVertexShader->addUniform(sizeof(float)).buildUniforms();
	}

	skyFragmentShader = new VKShader(skyFragmentBinaryContent);

	// create pipeline
	skyPipeline = aa::VKPipelineBuilder()
		.setVertexShader(skyVertexShader).setFragmentShader(skyFragmentShader).build();

}

void VulkanGraphicsFactory::buildMeshPrereq() {
	auto asteroidVertexBinaryContent	= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-3d_mesh.spv"
	);
	auto asteroidFragmentBinaryContent	= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-3d_mesh.spv"
	);

	tex = new VKTexture();
	tex->loadColormap("D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader\\Rock02_BaseColor.tga");

	{	// bind vertex shader data
		meshVertexShader = new VKVertexShader(asteroidVertexBinaryContent);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT,    2);
		meshVertexShader->addUniform(16 * sizeof(float)).addTextureUniform(tex).buildUniforms();
	}

	meshFragmentShader = new VKShader(asteroidFragmentBinaryContent);

	meshPipeline = VKPipelineBuilder()
		.setVertexShader(meshVertexShader).setFragmentShader(meshFragmentShader).build();

}
