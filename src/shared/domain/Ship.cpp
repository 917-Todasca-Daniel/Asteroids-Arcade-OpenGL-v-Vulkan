#include "Ship.h"

#include <iostream>

#include "window_constants.hpp"

#include "Mesh.h"

// dev code
#include "collision/CollisionShape.h"
#include "util/UInput.h"


using namespace aa;


Spaceship::Spaceship(
	LogicObject* parent, 
	Vector3d position, 
	Mesh* mesh, 
	CollisionShape* collision
) : Object3d(parent, position), shipMesh(mesh),
	collisionShape(collision),
	physics {
		Vector3d	(.0f, -40.0f, .0f),
		Vector3d	(.0f, .0f, .0f),
		Vector3d	(.0f, .0f, .0f),
		Vector3d	(.0f, .0f, .0f),
		Quaternion	(1.0f, 0.0f, .0f, .0f),
		Quaternion	(1.0f, .0f, 0.0f, 2.0f),
		Quaternion	(1.0f, 1.0f, .0f, .0f),
		Quaternion	(1.0f, .0f, .0f, 2.0f)
	}
{

}

Spaceship::~Spaceship()
{
	if (shipMesh)		delete shipMesh;
	if (collisionShape) delete collisionShape;

}


void Spaceship::kill()
{
	shipMesh->kill();

}

void Spaceship::init()
{
	Object3d::init();

	physics.rotation.normalize();
	physics.rotationAcceleration.normalize();

	shipMesh->init();
	shipMesh->setRotation(physics.rotation);
	shipMesh->setPosition(position);

}

void Spaceship::loop(float lap)
{
	Object3d::loop(lap);

	{	// handling input - changing direction (left/right)
		int rotationDirection = (UInput::isLeftKeyPressed() ? 1 : 0) + (UInput::isRightKeyPressed() ? -1 : 0);

		auto frameRotation = physics.rotationAcceleration * rotationDirection * lap;
		frameRotation.normalize();
		frameRotation = frameRotation * physics.rotation;
		frameRotation.normalize();

		physics.rotation = frameRotation;
	}

	{
		int rotationDirection = (UInput::isLeftKeyPressed() ? 1 : 0) + (UInput::isRightKeyPressed() ? -1 : 0);

		auto frameRotation = physics.meshRotationAcceleration * rotationDirection * lap;
		frameRotation.normalize();
		frameRotation = frameRotation * physics.meshRotation;
		frameRotation.normalize();

		physics.meshRotation = frameRotation;

		sideacc += rotationDirection * lap;
		if (rotationDirection == 0) {
			float inertia = lap * .5f;
			if (inertia > abs(sideacc)) {
				sideacc = 0.0f;
			}
			else {
				if (sideacc < 0) {
					sideacc += inertia;
				}
				else {
					sideacc -= inertia;
				}
			}
		}
		if (sideacc > 0.5f) sideacc = 0.5f;
		if (sideacc < -0.5f) sideacc = -0.5f;

		if (sideacc != .0f) {
			auto sideRotation = Quaternion(1.0f, .0f, .2f, 0.75f) * sideacc;
			sideRotation.normalize();
			frameRotation = frameRotation * sideRotation;
			frameRotation.normalize();
		}

		shipMesh->setRotation(frameRotation);
	}

	{	// handling input - moving forward
		int inputPower = (UInput::isUpKeyPressed() ? -1 : 0);

		auto additiveFrameMovement = physics.forwardAcceleration * lap * inputPower;
		
		physics.forwardSpeed += physics.rotation * additiveFrameMovement;

		float ratio = 1.0f;
		float maxSpeed = 12.0f;
		auto sp = physics.forwardSpeed;
		float speed = (sp.x * sp.x + sp.y * sp.y + sp.z * sp.z);
		if (speed > maxSpeed * maxSpeed) {
			ratio = sqrt(speed) / maxSpeed;
			physics.forwardSpeed.x /= ratio;
			physics.forwardSpeed.y /= ratio;
			physics.forwardSpeed.z /= ratio;
		}

		if (inputPower == 0) {
			physics.forwardSpeed = physics.forwardSpeed * (1.0f - lap);
		}

		position += physics.forwardSpeed;
		shipMesh->setPosition(position);
	}


	if (position.x > 5300) position.x = -5300;
	if (position.x < -5300) position.x = 5300;

	if (position.y > 5300) position.y = -5300;
	if (position.y < -5300) position.y = 5300;

}

void Spaceship::draw()
{
	Object3d::draw();

	shipMesh->draw();

}
