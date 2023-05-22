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
	asteroidMesh->setRotation(Quaternion(0, 0.01, 0.1, 0.01));
}

void Asteroid::loop(float lap)
{
	Object3d::loop(lap);

	position.x += lap * 1000;
	position.y -= lap * 1000;

	asteroidMesh->setPosition(position);
	asteroidMesh->getRotation().w += lap;
}
