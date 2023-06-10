#pragma once

#include "Object3d.h"

#include "data/Quaternion.hpp"


namespace aa
{

    // forward declarations
    class Mesh;
    class CollisionShape;


    struct SpaceshipPhysics {
        // amount of forward acceleration per second
        Vector3d forwardAcceleration;
        // maximum forward speed
        Vector3d forwardTopSpeed;
        // current forward speed
        Vector3d forwardSpeed;

        Vector3d friction;

        // current rotation, helps with determining direction
        Quaternion rotation;
        // amount of rotation per second
        Quaternion rotationAcceleration;

        // current rotation, helps with determining direction
        Quaternion meshRotation;
        // amount of rotation per second
        Quaternion meshRotationAcceleration;

    };


    // logic class for the player's spaceship
    class Spaceship : public Object3d
    {

    public:
        Spaceship(
            LogicObject*    parent, 
            Vector3d        position,
            Mesh*           mesh,
            CollisionShape* collision
        );
        virtual ~Spaceship();

        virtual void kill() override;

        virtual void init();

        virtual void loop(float lap) override;

        virtual void draw();


    protected:
        Mesh*               shipMesh;
        CollisionShape*     collisionShape;

        SpaceshipPhysics    physics;

        float               sideacc = .0f;

    };

}
