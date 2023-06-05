#pragma once

#include "Object3d.h"

#include "data/Quaternion.hpp"


namespace aa
{

    // forward declarations
    class Mesh;
    class CollisionShape;

    // logic class for the asteroids inside the game
    class Asteroid : public Object3d
    {

    public:
        Asteroid(
            LogicObject*    parent, 
            Vector3d        position,
            Mesh*           mesh,
            CollisionShape* collision,
            Vector3d        acceleration  = Vector3d(1000, 1000, 0),
            Quaternion      initRotation  = Quaternion(1, 0, 0, 0),
            Quaternion      frameRotation = Quaternion(1, 0, 1, 0)
        );
        virtual ~Asteroid();

        virtual void kill() override;

        virtual void init();

        virtual void loop(float lap) override;

        virtual void draw();


    protected:
        Mesh*           asteroidMesh;
        Vector3d        acceleration;
        Quaternion      initRotation;
        Quaternion      frameRotation;
        CollisionShape* collisionShape;

    };
}
