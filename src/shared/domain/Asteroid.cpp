#include "Asteroid.h"

#include <iostream>

// dev code
#include "GLMesh.h"

using namespace aa;


Asteroid::Asteroid(LogicObject* parent, Vector3d position, Mesh *mesh) 
	: Object3d(parent, position), asteroidMesh(mesh)
{
	
}

Asteroid::~Asteroid()
{

}


void Asteroid::kill() 
{
	asteroidMesh->kill();
}

void Asteroid::init()
{
	asteroidMesh->init();
	asteroidMesh->setRotation(Quaternion(1, -1, 0, 0));
}

void Asteroid::loop(float lap)
{
	Object3d::loop(lap);

	asteroidMesh->setPosition(position);
	Quaternion quat(1, 0, lap, 0);
	quat = quat * asteroidMesh->getRotation();
	quat.normalize();
	asteroidMesh->setRotation(quat);
}
