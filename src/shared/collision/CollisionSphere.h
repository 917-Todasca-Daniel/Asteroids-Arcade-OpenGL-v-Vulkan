#pragma once

#include "CollisionShape.h"


namespace aa
{

    class CollisionSphere : public CollisionShape
    {

    public:
        CollisionSphere(
            Object3d*      parent,
            CollisionLayer layer,
            int			   collideableLayers,
            float		   radius
        );
        virtual ~CollisionSphere();

        virtual bool collidesWith(CollisionShape* other) = 0;

        virtual float getRadius() const {
            return radius;
        }


    private:
        float radius;

    };
}
