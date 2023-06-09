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
#include "domain/Ship.h"
#include "domain/Laser.h"

#include "collision/CollisionHull.h"
#include "collision/CollisionSphere.h"

#include "util/UFile.h"
#include "util/URand.h"

#include "data/Vector3d.h"

#include "window_constants.hpp"

#include <iostream>

using namespace aa;

#define SCALE_MIN 0.4f
#define SCALE_MAX 0.7f

#define FBX_ASTEROID1 "D:/licenta/dev/app/res/shared/fbx/cgtrader/rock01.FBX"
#define FBX_ASTEROID2 "D:/licenta/dev/app/res/shared/fbx/cgtrader/rock02.FBX"
#define FBX_SHIP	  "D:/licenta/dev/app/res/shared/fbx/cgtrader/ship3.fbx"

#define HULL_ASTEROID1 "D:/licenta/dev/Convex-hull-master/data/mesh-1.out"
#define HULL_ASTEROID2 "D:/licenta/dev/Convex-hull-master/data/mesh-2.out"

#define VK_VERTEX_SHADER		"D:/licenta/dev/app/res/vulkan/shaders/v-3d_mesh.spv"
#define VK_FRAG_SHADER			"D:/licenta/dev/app/res/vulkan/shaders/f-3d_mesh.spv"
#define VK_VERTEX_SHADER_INST	"D:/licenta/dev/app/res/vulkan/shaders/v-instanced_3d_mesh.spv"
#define VK_FRAG_SHADER_INST		"D:/licenta/dev/app/res/vulkan/shaders/f-instanced_3d_mesh.spv"
#define VK_FRAG_SHADER_LASER	"D:/licenta/dev/app/res/vulkan/shaders/f-3d_laser.spv"

#define GL_VERTEX_SHADER		"v_3d_mesh"
#define GL_FRAG_SHADER			"f_3d_mesh"
#define GL_VERTEX_SHADER_INST	"v_instanced_3d_mesh"
#define GL_FRAG_SHADER_INST		"f_instanced_3d_mesh"

#define GL_FRAG_SHADER_LASER	"f_laser"

#define SHIP_SCALE				0.25f

#define COUNT_LASERS			100



#ifdef IMPL_VULKAN
	SpaceshipFactory* SpaceshipFactory::instance = (SpaceshipFactory*) new VulkanSpaceshipFactory();
#endif
#ifdef IMPL_OPENGL
	SpaceshipFactory* SpaceshipFactory::instance = (SpaceshipFactory*) new OpenGLSpaceshipFactory();
#endif



#ifdef IMPL_VULKAN
	LaserFactory* LaserFactory::instance = (LaserFactory*) new VulkanLaserFactory();
#endif
#ifdef IMPL_OPENGL
	LaserFactory* LaserFactory::instance = (LaserFactory*) new OpenGLLaserFactory();
#endif



LaserFactory::LaserFactory() {

}

LaserFactory::~LaserFactory() {
	for (auto& laser : lasers) {
		delete laser;
	}

}

LaserFactory* LaserFactory::getInstance() {
	return instance;
}

std::vector<Laser*> LaserFactory::buildLasers() {
	for (int i = 0; i < COUNT_LASERS; i++) {
		auto mesh = createMesh();
		CollisionShape* collision = new CollisionSphere(
			nullptr,
			COLLISION_GUN, COLLISION_ASTEROID,
			1000.0f
		);
		mesh->vertices.insert(
			mesh->vertices.end(),
			{ 
				 3,  100, 0.5f, 0, 0, 0, 0, 0,
				 3, -100, 0.5f, 0, 0, 0, 0, 0,
				-3, -100, 0.5f, 0, 0, 0, 0, 0,
				-3,  100, 0.5f, 0, 0, 0, 0, 0
			}
		);
		mesh->indices.insert(
			mesh->indices.end(),
			{ 
				0, 1, 2, 2, 3, 0
			}
		);
		mesh->init();
		auto laser = new Laser(AA_ROOT, Vector3d(0, 0, 0), mesh, collision);
		lasers.push_back(laser);
	}

	return lasers;
}

void LaserFactory::draw() {

}



OpenGLLaserFactory::OpenGLLaserFactory() : LaserFactory() {

}

OpenGLLaserFactory::~OpenGLLaserFactory() {
	for (auto& shader : shaders) {
		delete shader;
	}

}

Mesh* OpenGLLaserFactory::createMesh() {
	auto shader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader(GL_VERTEX_SHADER)
		.setFragmentShader(GL_FRAG_SHADER_LASER)
		.build();
	shaders.push_back(shader);

	return new GLMesh(AA_ROOT, Vector3d(0, 0, 0), shader);
}



VulkanLaserFactory::VulkanLaserFactory() : LaserFactory() {

}

VulkanLaserFactory::~VulkanLaserFactory() {
	for (auto& shader : shaders) {
		delete shader;
	}
	for (auto& pipeline : pipelines) {
		delete pipeline;
	}

}

Mesh* VulkanLaserFactory::createMesh() {
	auto vertexBinaryContent = UFile::readBinaryFileContent(
		VK_VERTEX_SHADER
	);
	auto fragmentBinaryContent = UFile::readBinaryFileContent(
		VK_FRAG_SHADER_LASER
	);

	// bind vertex shader data
	auto meshVertexShader = new VKVertexShader(vertexBinaryContent);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT, 2);
	meshVertexShader->addUniform(16 * sizeof(float)).buildUniforms();

	auto meshFragmentShader = new VKShader(fragmentBinaryContent);

	auto meshPipeline = VKPipelineBuilder()
		.setVertexShader(meshVertexShader).setFragmentShader(meshFragmentShader).build();


	VKMesh* mesh = new VKMesh(AA_ROOT, aa::Vector3d(0, 0, 0.0f), meshPipeline);

	shaders.push_back(meshFragmentShader);
	shaders.push_back(meshVertexShader);
	pipelines.push_back(meshPipeline);

	return mesh;
}



SpaceshipFactory::SpaceshipFactory() : spaceship(nullptr), shipCollision(nullptr)
{

}
SpaceshipFactory::~SpaceshipFactory() {
	if (shipCollision) delete shipCollision;
}

SpaceshipFactory* SpaceshipFactory::getFactory() {
	return instance;
}


Object3d* SpaceshipFactory::buildSpaceship(Mesh* mesh)
{
	CollisionShape* collisionSphere = new CollisionSphere(
		(Object3d*) nullptr, COLLISION_SHIP, COLLISION_ASTEROID,
		mesh->getRadius() * 0.5f
	);
	shipCollision = collisionSphere;

	return spaceship = new Spaceship(
		AA_ROOT, 
		Vector3d(0, 0, 0), 
		mesh, 
		collisionSphere
	);
}


OpenGLSpaceshipFactory::OpenGLSpaceshipFactory() : shipTex(nullptr)
{

}
OpenGLSpaceshipFactory::~OpenGLSpaceshipFactory() {
	if (shipTex) delete shipTex;
	for (auto& shader : shaders) {
		delete shader;
	}
}

Object3d* OpenGLSpaceshipFactory::buildSpaceship() 
{
	shipTex = GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
	)	.setColorFile("Ship_BaseColor2", "jpg")
		.build();

	GLShader* meshShader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader(GL_VERTEX_SHADER)
		.setFragmentShader(GL_FRAG_SHADER)
		.addUniformTex("u_Texture", shipTex)
		.build();
	shaders.push_back(meshShader);


	float scale = SHIP_SCALE;
	GLMesh* obj = new GLMesh(AA_ROOT, Vector3d(0, 0, 0), meshShader);
	obj->loadFromFbx(FBX_SHIP, scale);

	return SpaceshipFactory::buildSpaceship(obj);
}


VulkanSpaceshipFactory:: VulkanSpaceshipFactory() : shipTex(nullptr)
{

}
VulkanSpaceshipFactory::~VulkanSpaceshipFactory() 
{
	if (shipTex) delete shipTex;
	for (auto& shader : shaders) {
		delete shader;
	}
	for (auto& pipeline : pipelines) {
		delete pipeline;
	}
}

Object3d* VulkanSpaceshipFactory::buildSpaceship()
{
	shipTex = new VKTexture();
	shipTex->loadColormap("D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader\\Ship_BaseColor2.jpg");

	auto shipVertexBinaryContent = UFile::readBinaryFileContent(
		VK_VERTEX_SHADER
	);
	auto shipFragmentBinaryContent = UFile::readBinaryFileContent(
		VK_FRAG_SHADER
	);

	// bind vertex shader data
	auto meshVertexShader = new VKVertexShader(shipVertexBinaryContent);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT, 2);
	meshVertexShader->addUniform(16 * sizeof(float))
		.addTextureUniform(shipTex).buildUniforms();

	auto meshFragmentShader = new VKShader(shipFragmentBinaryContent);

	auto meshPipeline = VKPipelineBuilder()
		.setVertexShader(meshVertexShader).setFragmentShader(meshFragmentShader).build();

	float scale = SHIP_SCALE;
	VKMesh* mesh = new VKMesh(AA_ROOT, aa::Vector3d(0, 0, 0.0f), meshPipeline);
	mesh->loadFromFbx(FBX_SHIP, scale);

	shaders.push_back(meshFragmentShader);
	shaders.push_back(meshVertexShader);
	pipelines.push_back(meshPipeline);

	return SpaceshipFactory::buildSpaceship(mesh);
}


#ifdef IMPL_VULKAN
	GameFactory* GameFactory::instance = MESH_INSTANCING_ON ?
		(GameFactory*) new VulkanInstacedGraphicsFactory() :
		(GameFactory*) new VulkanGraphicsFactory();
#endif
#ifdef IMPL_OPENGL
	GameFactory* GameFactory::instance = MESH_INSTANCING_ON ?
		(GameFactory*) new OpenGLInstancedGraphicsFactory() :
		(GameFactory*) new OpenGLGraphicsFactory();
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

Object3d* GameFactory::buildAsteroid(Mesh* mesh, CollisionShape *collision) {
	Vector3d pos		= URand::randPosition();
	Vector3d acc		= URand::randAcceleration();
	Quaternion rot		= URand::randRotation();
	Quaternion frameRot = URand::randTimeRotation();

	float type = URand::randBetween(.0f, 4.0f);
	if (type < 1.0f) {
		;
	} else
	if (type < 2.0f) {
		pos.y = -pos.y;
		acc.y = -acc.y;
	} else
	if (type < 3.0f) {
		pos.x = -pos.x;
		acc.x = -acc.x;
	} else
	if (type < 4.0f) {
		pos.x = -pos.x;
		acc.x = -acc.x;
		pos.y = -pos.y;
		acc.y = -acc.y;
	}

	Asteroid* asteroid = new Asteroid(AA_ROOT, pos, mesh, collision, acc, rot, frameRot);

	if (collision) {
		collisions.push_back(collision);
		collision->setParent(asteroid);
	}

	return asteroid;
}

CollisionShape* GameFactory::createLargeAsteroidCollision(float scale)
{
	std::vector <Vector3d> points = UFile::readHull(
		HULL_ASTEROID1
	);
	CollisionHull* collisionHull = new CollisionHull(
		nullptr, COLLISION_ASTEROID, COLLISION_SHIP | COLLISION_GUN,
		points, scale
	);
	return collisionHull;
}

CollisionShape* GameFactory::createSmallAsteroidCollision(float scale)
{
	std::vector <Vector3d> points = UFile::readHull(
		HULL_ASTEROID2
	);
	CollisionHull* collisionHull = new CollisionHull(
		nullptr, COLLISION_ASTEROID, COLLISION_GUN | COLLISION_SHIP,
		points, scale
	);
	return collisionHull;
}


void GameFactory::draw() 
{
	if (SHIP_FACTORY->shipCollision) {
		SHIP_FACTORY->shipCollision->updateAfterLoop();
	}
	for (auto& coll : collisions) {
		coll->updateAfterLoop();
	}

	for (auto& c1 : collisions) {
		auto asteroid = (Asteroid*) c1->getParent();
		if (!asteroid->activity()) continue;

		if (SHIP_FACTORY->shipCollision) {
			if (c1->collidesWith(SHIP_FACTORY->shipCollision)) {
				((Spaceship*) SHIP_FACTORY->spaceship)->onAsteroidCollision();
			}
		}
		for (auto& laser : LASER_FACTORY->lasers) {
			if (!laser->activity()) continue;
			if (c1->collidesWith(laser->getCollision())) {
				laser->onAsteroidCollision(asteroid);
			}
		}
	}
}



OpenGLGraphicsFactory::OpenGLGraphicsFactory() : asteroidTex(nullptr), asteroid(nullptr)
{

}

OpenGLGraphicsFactory::~OpenGLGraphicsFactory()
{
	for (auto& shader : shaders) {
		delete shader;
	}
	if (asteroid)	 delete asteroid;
	if (asteroidTex) delete asteroidTex;

}


void OpenGLGraphicsFactory::draw()
{
	GameFactory::draw();

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
	if (asteroidTex == nullptr) {
		buildMeshPrereq();
	}

	GLShader* meshShader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader	(GL_VERTEX_SHADER)
		.setFragmentShader	(GL_FRAG_SHADER)
		.addUniformTex		("u_Texture", asteroidTex)
		.build();
	shaders.push_back(meshShader);


	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	GLMesh* obj  = new GLMesh(AA_ROOT, Vector3d(0, 0, 0), meshShader);
	obj->loadFromFbx(FBX_ASTEROID1, scale);
	std::cout << obj->getRadius();

	return GameFactory::buildAsteroid(obj, createLargeAsteroidCollision(scale));
}

Object3d* OpenGLGraphicsFactory::buildSmallAsteroid() {
	if (asteroidTex == nullptr) {
		buildMeshPrereq();
	}

	GLShader* meshShader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader	(GL_VERTEX_SHADER)
		.setFragmentShader	(GL_FRAG_SHADER)
		.addUniformTex		("u_Texture", asteroidTex)
		.build();
	shaders.push_back(meshShader);

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	GLMesh* obj  = new GLMesh(AA_ROOT, Vector3d(0, 0, 0), meshShader);
	obj->loadFromFbx(FBX_ASTEROID2, scale);
	std::cout << obj->getRadius();

	return GameFactory::buildAsteroid(obj, createSmallAsteroidCollision(scale));
}


void OpenGLGraphicsFactory::buildMeshPrereq() {
	asteroidTex = GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
	).setColorFile("Rock02_BaseColor", "tga")
		.build();
}



OpenGLInstancedGraphicsFactory::OpenGLInstancedGraphicsFactory() 
	: asteroidTex(nullptr), skyShader(nullptr)
{

}

OpenGLInstancedGraphicsFactory::~OpenGLInstancedGraphicsFactory()
{
	for (int i = 0; i < 2; i++) {
		if (asteroidInstance[i])	delete asteroidInstance[i];
		if (asteroidShader[i])		delete asteroidShader[i];
	}

	if (asteroidTex)		delete asteroidTex;
	if (skyShader)			delete skyShader;

}


void OpenGLInstancedGraphicsFactory::draw()
{
	GameFactory::draw();

	if (asteroidInstance[0]) {
		asteroidInstance[0]->draw();
	}
	if (asteroidInstance[1]) {
		asteroidInstance[1]->draw();
	}

}


Object3d* OpenGLInstancedGraphicsFactory::buildSky(float height, float width, float* time) {
	auto shader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader	("v_perlin_sky")
		.setFragmentShader	("f_perlin_sky")
		.build();

	auto rectangle = new GLRectangle(AA_ROOT, Vector3d(width*0.5f, height*0.5f, 0), height, width, shader);

	shader->addUniform1iRef("u_Time", time);
	skyShader = shader;

	return rectangle;
}

Object3d* OpenGLInstancedGraphicsFactory::buildLargeAsteroid() {
	if (asteroidTex == nullptr) {
		buildMeshPrereq();
	}

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	Mesh* obj = new GLMeshInstance(
		asteroidInstance[0], Vector3d(0, 0, 0),
		asteroidsLargeIndex ++, scale
	);

	return GameFactory::buildAsteroid(obj, createLargeAsteroidCollision(scale));
}

Object3d* OpenGLInstancedGraphicsFactory::buildSmallAsteroid() {
	if (asteroidTex == nullptr) {
		buildMeshPrereq();
	}

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	Mesh* obj = new GLMeshInstance(
		asteroidInstance[1], Vector3d(0, 0, 0),
		asteroidsSmallIndex ++, scale
	);

	return GameFactory::buildAsteroid(obj, createSmallAsteroidCollision(scale));
}


void OpenGLInstancedGraphicsFactory::buildMeshPrereq() {
	asteroidTex = GLTextureFileBuilder(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader"
	).setColorFile("Rock02_BaseColor", "tga")
		.build();

	for (int i = 0; i < 2; i++) {
		asteroidShader[i] = aa::GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
			.setVertexShader	(GL_VERTEX_SHADER_INST)
			.setFragmentShader	(GL_FRAG_SHADER_INST)
			.addUniformTex		("u_Texture", asteroidTex)
			.build();

		asteroidInstance[i] = new GLInstancedMesh(AA_ROOT, asteroidShader[i], NUM_ASTEROIDS);
		if (i == 0) {
			asteroidInstance[i]->loadFromFbx(FBX_ASTEROID1);
		}
		else {
			asteroidInstance[i]->loadFromFbx(FBX_ASTEROID2);
		}
		asteroidInstance[i]->init();
	}
}



VulkanGraphicsFactory::VulkanGraphicsFactory()
	: asteroidTex(nullptr)
{

}

VulkanGraphicsFactory::~VulkanGraphicsFactory()
{
	if (asteroidTex) delete asteroidTex;
	for (auto& shader : shaders) {
		delete shader;
	}
	for (auto& pipeline : pipelines) {
		delete pipeline;
	}
}


Object3d* VulkanGraphicsFactory::buildSky(float height, float width, float* time) {
	auto skyVertexBinaryContent		= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-perlin_sky.spv"
	);
	auto skyFragmentBinaryContent	= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/f-perlin_sky.spv"
	);

	// bind vertex shader data
	auto skyVertexShader = new VKVertexShader(skyVertexBinaryContent);
	skyVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	skyVertexShader->addUniform(sizeof(float)).buildUniforms();

	auto skyFragmentShader = new VKShader(skyFragmentBinaryContent);

	// create pipeline
	auto skyPipeline = VKPipelineBuilder()
		.setVertexShader(skyVertexShader).setFragmentShader(skyFragmentShader).build();

	pipelines.push_back(skyPipeline);
	shaders.push_back(skyVertexShader);
	shaders.push_back(skyFragmentShader);

	return new VKRectangle(AA_ROOT, Vector3d(width*0.5f, height*0.5f, 0), height, width, skyPipeline);
}


Object3d* VulkanGraphicsFactory::buildLargeAsteroid() {
	return buildAsteroid(FBX_ASTEROID1, HULL_ASTEROID1);
}
Object3d* VulkanGraphicsFactory::buildSmallAsteroid() {
	return buildAsteroid(FBX_ASTEROID2, HULL_ASTEROID2);
}

Object3d* VulkanGraphicsFactory::buildAsteroid(const char *fbxPath, const char* hullPath) {
	if (asteroidTex == nullptr) {
		buildMeshPrereq();
	}

	auto asteroidVertexBinaryContent	= UFile::readBinaryFileContent(
		VK_VERTEX_SHADER
	);
	auto asteroidFragmentBinaryContent	= UFile::readBinaryFileContent(
		VK_FRAG_SHADER
	);

	// bind vertex shader data
	auto meshVertexShader = new VKVertexShader(asteroidVertexBinaryContent);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
	meshVertexShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT,    2);
	meshVertexShader->addUniform(16 * sizeof(float))
		.addTextureUniform(asteroidTex).buildUniforms();

	auto meshFragmentShader = new VKShader(asteroidFragmentBinaryContent);

	auto meshPipeline = VKPipelineBuilder()
		.setVertexShader(meshVertexShader).setFragmentShader(meshFragmentShader).build();

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);

	VKMesh* mesh = new VKMesh(AA_ROOT, aa::Vector3d(0, 0, 0.0f), meshPipeline);
	mesh->loadFromFbx(fbxPath, scale);

	shaders.push_back(meshFragmentShader);
	shaders.push_back(meshVertexShader);
	pipelines.push_back(meshPipeline);

	std::vector <Vector3d> points = UFile::readHull(
		hullPath
	);
	CollisionHull* collisionHull = new CollisionHull(
		nullptr, COLLISION_ASTEROID, COLLISION_GUN | COLLISION_SHIP,
		points, scale
	);

	return GameFactory::buildAsteroid(mesh, collisionHull);

}

void VulkanGraphicsFactory::draw() 
{
	GameFactory::draw();

}


void VulkanGraphicsFactory::buildMeshPrereq() {
	asteroidTex = new VKTexture();
	asteroidTex->loadColormap("D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader\\Rock02_BaseColor.tga");

}



VulkanInstacedGraphicsFactory::VulkanInstacedGraphicsFactory()
	: asteroidTex(nullptr), skyVertexShader(nullptr),
	skyFragmentShader(nullptr),
	skyPipeline(nullptr)
{

}

VulkanInstacedGraphicsFactory::~VulkanInstacedGraphicsFactory()
{
	if (asteroidTex)		delete asteroidTex;
	if (skyVertexShader)	delete skyVertexShader;
	if (skyFragmentShader)	delete skyFragmentShader;
	if (skyPipeline)		delete skyPipeline;

	for (int i = 0; i < 2; i++) {
		if (meshPipeline[i])		delete meshPipeline[i];
		if (asteroidInstance[i])    delete asteroidInstance[i];
		if (meshFragmentShader[i])  delete meshFragmentShader[i];
		if (meshVertexShader[i])	delete meshVertexShader[i];
	}

}


Object3d* VulkanInstacedGraphicsFactory::buildSky(float height, float width, float* time) {
	if (skyPipeline == nullptr) {
		buildSkyPrereq();
	}

	return new VKRectangle(
		AA_ROOT, 
		Vector3d(width*0.5f, height*0.5f, 0), 
		height, 
		width, 
		skyPipeline
	);
}

Object3d* VulkanInstacedGraphicsFactory::buildLargeAsteroid() {
	if (asteroidTex == nullptr) {
		buildAsteroidPrereq();
	}

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	Mesh* mesh = new VKMeshInstance(
		asteroidInstance[0], aa::Vector3d(0, 0, 0.0f),
		asteroidsLargeCount ++, scale
	);

	return GameFactory::buildAsteroid(mesh, createLargeAsteroidCollision(scale));

}

Object3d* VulkanInstacedGraphicsFactory::buildSmallAsteroid() {
	if (asteroidTex == nullptr) {
		buildAsteroidPrereq();
	}

	float scale = URand::randBetween(SCALE_MIN, SCALE_MAX);
	Mesh* mesh = new VKMeshInstance(
		asteroidInstance[1], aa::Vector3d(0, 0, 0.0f),
		asteroidsSmallCount++, scale
	);

	return GameFactory::buildAsteroid(mesh, createSmallAsteroidCollision(scale));

}

void VulkanInstacedGraphicsFactory::draw()
{
	GameFactory::draw();

	if (asteroidInstance[0]) {
		asteroidInstance[0]->draw();
	}
	if (asteroidInstance[1]) {
		asteroidInstance[1]->draw();
	}

}


void VulkanInstacedGraphicsFactory::buildAsteroidPrereq() {
	asteroidTex = new VKTexture();
	asteroidTex->loadColormap(
		"D:\\licenta\\dev\\app\\res\\shared\\textures\\cgtrader\\Rock02_BaseColor.tga"
	);
	buildMeshPrereq(
		meshPipeline[0], asteroidInstance[0],
		meshVertexShader[0], meshFragmentShader[0],
		FBX_ASTEROID1
	);
	buildMeshPrereq(
		meshPipeline[1], asteroidInstance[1],
		meshVertexShader[1], meshFragmentShader[1],
		FBX_ASTEROID2
	);
}

void VulkanInstacedGraphicsFactory::buildMeshPrereq(
	VKPipeline*&		meshPipeline,
	VKInstancedMesh*&	asteroidInstance,
	VKVertexShader*&	meshVertexShader,
	VKShader*&			meshFragmentShader,
	const char*			fbxPath
) {
	auto asteroidVertexBinaryContent	= UFile::readBinaryFileContent(
		VK_VERTEX_SHADER_INST
	);
	auto asteroidFragmentBinaryContent	= UFile::readBinaryFileContent(
		VK_FRAG_SHADER_INST
	);

	{	// bind vertex shader data
		meshVertexShader = new VKVertexShader(asteroidVertexBinaryContent);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32B32_SFLOAT, 3);
		meshVertexShader->addBinding<float>(VK_FORMAT_R32G32_SFLOAT, 2);
		meshVertexShader->addInstanceBinding<float>(VK_FORMAT_R32G32B32A32_SFLOAT, 4);
		meshVertexShader->addInstanceBinding<float>(VK_FORMAT_R32G32B32A32_SFLOAT, 4);
		meshVertexShader->addInstanceBinding<float>(VK_FORMAT_R32G32B32A32_SFLOAT, 4);
		meshVertexShader->addInstanceBinding<float>(VK_FORMAT_R32G32B32A32_SFLOAT, 4);
		meshVertexShader->addUniform(1 * sizeof(float), 1)
			.addTextureUniform(asteroidTex).buildUniforms();
	}

	meshFragmentShader = new VKShader(asteroidFragmentBinaryContent);
	meshPipeline = VKPipelineBuilder()
		.setVertexShader(meshVertexShader).setFragmentShader(meshFragmentShader).build();

	asteroidInstance = new VKInstancedMesh(AA_ROOT, meshPipeline, NUM_ASTEROIDS);
	asteroidInstance->loadFromFbx(fbxPath);
	asteroidInstance->init();

}

void VulkanInstacedGraphicsFactory::buildSkyPrereq() {
	auto skyVertexBinaryContent		= UFile::readBinaryFileContent(
		"D:/licenta/dev/app/res/vulkan/shaders/v-perlin_sky.spv"
	);
	auto skyFragmentBinaryContent = UFile::readBinaryFileContent(
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
