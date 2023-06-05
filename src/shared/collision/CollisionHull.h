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

        virtual void updateAfterLoop() override;


    protected:
        std::vector <Vector3d> points;
        std::vector <Vector3d> framePoints;

    };
}
