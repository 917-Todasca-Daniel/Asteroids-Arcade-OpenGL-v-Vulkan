#pragma once

#include "Object3d.h"

#include "data/Quaternion.hpp"


namespace aa
{

    // forward declarations
    class Mesh;
    class CollisionShape;
    class Asteroid;


    // logic class for the player's spaceship projectiles
    class Laser : public Object3d
    {

    public:
        Laser(
            LogicObject*    parent, 
            Vector3d        position,
            Mesh*           mesh,
            CollisionShape* collision
        );
        virtual ~Laser();

        void onAsteroidCollision(Asteroid*);

        virtual void kill() override;

        virtual void init();

        virtual void loop(float lap) override;

        virtual void draw();

        void spawn(Vector3d position, Quaternion forwardDirection);

        CollisionShape* getCollision() const {
            return collisionShape;
        }

        bool activity() {
            return isActive;
        }


    protected:
        Mesh*               laserMesh;
        CollisionShape*     collisionShape;

        Vector3d            forwardSpeed;
        bool                isActive;
        float               activityTimer;

    };

}
