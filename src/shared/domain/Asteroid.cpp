#include "Asteroid.h"

#include <iostream>

// dev code
#include "GLMesh.h"

using namespace aa;


Asteroid::Asteroid(LogicObject* parent, Vector3d position, Mesh *mesh, Vector3d acceleration)
	: Object3d(parent, position), asteroidMesh(mesh), acceleration(acceleration)
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
	asteroidMesh->setRotation(Quaternion(1, -1, 0, 0));

}

void Asteroid::loop(float lap)
{
	Object3d::loop(lap);

	position += acceleration * lap;
	asteroidMesh->setPosition(position);

	Quaternion frameRotation(1, 0, lap, 0);
	frameRotation = frameRotation * asteroidMesh->getRotation();
	frameRotation.normalize();
	asteroidMesh->setRotation(frameRotation);

}

void Asteroid::draw() 
{
	Object3d::draw();

	asteroidMesh->draw();

}
