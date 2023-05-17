#pragma once

#include "domain/Object3d.h"
#include "data/Quaternion.hpp"


namespace aa
{


    // loads and draws 3d objects
    // no rotation is applied by default and position is additive to vertex positions
    class Mesh : public Object3d
    {

    public:
        Mesh(
            LogicObject* parent,
            Vector3d     position
        );
        virtual ~Mesh();

        Quaternion& getRotation() {
            return this->rotation;
        }

        void setRotation(Quaternion rot) {
            this->rotation = rot;
        }


    protected:
        Quaternion rotation;

    };
}
