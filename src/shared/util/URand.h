#pragma once

#include "data/Vector3d.h"
#include "data/Quaternion.hpp"


namespace aa
{

    class URand {

    public:
        static float randBetween(float x, float y);

        static Vector3d randAcceleration();

        static Vector3d randPosition();

        static Quaternion randRotation();

        static Quaternion randTimeRotation();


    private:
        static int seed;

    };

}
