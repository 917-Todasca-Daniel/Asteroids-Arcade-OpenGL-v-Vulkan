#pragma once

#include "CollisionShape.h"

#include "data/Vector3d.h"


namespace aa
{

    class CollisionHull : public CollisionShape
    {

    public:
        CollisionHull(
            Object3d*      parent,
            CollisionLayer layer,
            int			   collideableLayers,
            const std::vector <Vector3d>& points,
            float scale = 1.0f
        );
        virtual ~CollisionHull();

        virtual bool collidesWith(CollisionShape* other);


    private:
        std::vector <Vector3d> points;

    };
}
