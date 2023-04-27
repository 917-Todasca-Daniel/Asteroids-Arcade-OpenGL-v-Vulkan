#include "Asteroid.h"

#include <iostream>

// dev code
#include "GLMesh.h"

using namespace aa;


Asteroid::Asteroid(LogicObject* parent, Vector3d position, GLMesh *mesh) 
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
}

void Asteroid::loop(float lap)
{
	Object3d::loop(lap);

	position.x += lap * 100;
	position.y += lap * 100;

	asteroidMesh->setPosition(position);
}
