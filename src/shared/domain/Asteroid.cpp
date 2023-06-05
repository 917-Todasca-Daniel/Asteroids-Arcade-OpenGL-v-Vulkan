#include "Asteroid.h"

#include <iostream>

#include "window_constants.hpp"

// dev code
#include "GLMesh.h"
#include "collision/CollisionShape.h"

using namespace aa;


Asteroid::Asteroid(
	LogicObject* parent, Vector3d position, 
	Mesh* mesh, CollisionShape* collision,
	Vector3d acceleration, Quaternion initRotation,
	Quaternion frameRotation
) : Object3d(parent, position), asteroidMesh(mesh), acceleration(acceleration), 
	initRotation(initRotation), frameRotation(frameRotation),
	collisionShape(collision)
{

}

Asteroid::~Asteroid()
{
	if (asteroidMesh) delete asteroidMesh;
	if (collisionShape) delete collisionShape;

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

	position += acceleration * lap * 2;
	asteroidMesh->setPosition(position);

	if (position.x > 6100) position.x = -6100;
	if (position.x < -6100) position.x = 6100;

	if (position.y > 6100) position.y = -6100;
	if (position.y < -6100) position.y = 6100;

	Quaternion rot = frameRotation * lap;
	rot = rot * asteroidMesh->getRotation();
	rot.normalize();

	asteroidMesh->setRotation(rot);

	collisionShape->setPosition(position);
	collisionShape->setRotation(rot);

}

void Asteroid::draw() 
{
	Object3d::draw();

	asteroidMesh->draw();

}
