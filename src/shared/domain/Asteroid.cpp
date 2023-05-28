#include "Asteroid.h"

#include <iostream>

// dev code
#include "GLMesh.h"

using namespace aa;


Asteroid::Asteroid(
	LogicObject* parent, Vector3d position, 
	Mesh *mesh, Vector3d acceleration, Quaternion initRotation,
	Quaternion frameRotation
) : Object3d(parent, position), asteroidMesh(mesh), acceleration(acceleration), 
	initRotation(initRotation), frameRotation(frameRotation)
{

}

Asteroid::~Asteroid()
{
	if (asteroidMesh) delete asteroidMesh;

}


void Asteroid::kill() 
{
	asteroidMesh->kill();
}

void Asteroid::init()
{
	Object3d::init();

	asteroidMesh->init();
	asteroidMesh->setRotation(initRotation);

}

void Asteroid::loop(float lap)
{
	Object3d::loop(lap);

	position += acceleration * lap;
	asteroidMesh->setPosition(position);

	Quaternion rot = frameRotation * lap;
	rot = rot * asteroidMesh->getRotation();
	rot.normalize();
	asteroidMesh->setRotation(rot);

}

void Asteroid::draw() 
{
	Object3d::draw();

	asteroidMesh->draw();

}
