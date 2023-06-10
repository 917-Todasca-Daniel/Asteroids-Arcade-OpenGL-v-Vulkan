#pragma once

#include "data/Vector3d.h"
#include "data/Quaternion.hpp"


namespace aa
{

    class Object3d;
    class Mesh;

    enum CollisionLayer {
        COLLISION_SHIP      = 1,
        COLLISION_ASTEROID  = 2,
        COLLISION_GUN       = 4
    };

    class CollisionShape
    {

    public:
        CollisionShape(
            Object3d*      parent,
            CollisionLayer objectLayer,
            int            collideableLayers,
            Mesh*          mesh
        );
        virtual ~CollisionShape();

        virtual bool collidesWith(CollisionShape* other) = 0;

        virtual void updateAfterLoop() {

        }

        void setPosition(Vector3d position) {
            this->position = position;
        }

        void setRotation(Quaternion rotation) {
            this->rotation = rotation;
        }

        CollisionLayer getCollisionLayer() const {
            return objectLayer;
        }

        Vector3d getPosition() const {
            return this->position;
        }

        //  delete all implicit constructors 
        CollisionShape()                  = delete;
        CollisionShape(const CollisionShape&)   = delete;
        CollisionShape(CollisionShape&&)        = delete;

        CollisionShape& operator = (const CollisionShape&)  = delete;
        CollisionShape& operator = (CollisionShape&&)       = delete;


    protected:
        Quaternion     rotation;
        Vector3d       position;
        CollisionLayer objectLayer;
        int            collideableLayers;
        Object3d*      parent;
        Mesh*          debugMesh;

    };
}
