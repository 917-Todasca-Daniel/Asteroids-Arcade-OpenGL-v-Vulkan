#include "Laser.h"

#include <iostream>

#include "window_constants.hpp"

#include "Mesh.h"

// dev code
#include "domain/Asteroid.h"
#include "collision/CollisionShape.h"
#include "util/UInput.h"

#define LASER_LIFESPAN 0.5f


using namespace aa;

Vector3d _forwardSpeed = Vector3d(0.f, 8000.0f, .0f);


Laser::Laser(
	LogicObject* parent, 
	Vector3d position, 
	Mesh* mesh, 
	CollisionShape* collision
) : Object3d(parent, position), laserMesh(mesh),
	collisionShape(collision), forwardSpeed(0, 0, 0),
	isActive(false), activityTimer(0)
{

}

Laser::~Laser()
{
	if (laserMesh)		delete laserMesh;
	if (collisionShape) delete collisionShape;

}


void Laser::onAsteroidCollision(Asteroid* asteroid)
{
	isActive = false;
	asteroid->despawn();

}


void Laser::kill()
{

}

void Laser::init()
{
	Object3d::init();

}

void Laser::loop(float lap)
{
	Object3d::loop(lap);

	if (isActive) {
		activityTimer += lap;

		if (activityTimer > LASER_LIFESPAN) {
			isActive = false;
		}
		else {
			auto additiveFrameMovement = forwardSpeed * lap;

			position += additiveFrameMovement;

			if (position.x > 5300) position.x = -5300;
			if (position.x < -5300) position.x = 5300;

			if (position.y > 5300) position.y = -5300;
			if (position.y < -5300) position.y = 5300;

			laserMesh->setPosition(position);

			if (collisionShape) {
				collisionShape->setPosition(position);
			}
		}
	}

}


void Laser::draw()
{
	Object3d::draw();

	if (isActive) {
		laserMesh->draw();
	}

}


void Laser::spawn(Vector3d position, Quaternion forwardDirection) {
	forwardSpeed = forwardDirection * _forwardSpeed;
	this->setPosition(position);

	isActive = true;
	activityTimer = 0.0f;

	laserMesh->setPosition(position);
	laserMesh->setRotation(forwardDirection);

	if (collisionShape) {
		collisionShape->setPosition(position);
	}

}
