#include "URand.h"

#include <random>
#include <time.h>
#include <iostream>

#include "window_constants.hpp"

using namespace aa;


#define ROT_X_MIN       600.0f
#define ROT_X_MAX       900.0f

#define Y_SCALE_LESS    1.5f
#define Y_SCALE_GREATER 2.0f


std::mt19937 rng(42);


float URand::randBetween(float x, float y) {
    std::uniform_real_distribution<float> distribution(x, y);
    float randomNumber = distribution(rng);

    return randomNumber;
}


Vector3d URand::randAcceleration() {
    float x = randBetween(ROT_X_MIN, ROT_X_MAX);
    float y = randBetween(x * Y_SCALE_LESS, x * Y_SCALE_GREATER);

    float type = randBetween(.0f, 1.0f);
    if (type < 0.5f) {
        return Vector3d(x, y, .0f);
    }
    return Vector3d(y, x, .0f);
}


Vector3d URand::randPosition() {
    float x = randBetween(- WINDOW_WIDTH * 2.5f, WINDOW_WIDTH * 2.5f);
    float y = randBetween(- WINDOW_HEIGHT * 4.5f, - WINDOW_HEIGHT * 4.0f);

    return Vector3d(x, y, 0);
}


Quaternion URand::randRotation() {
    float axis1 = randBetween(-3.0f, 3.0f);
    float axis2 = randBetween(-3.0f, 3.0f);
    float axis3 = randBetween(-0.3f, 0.3f);

    Quaternion quat(1, axis1, axis2, axis3);
    quat.normalize();
    return quat;
}


Quaternion URand::randTimeRotation() {
    float axis1 = randBetween(.5f, 1.0f);
    float axis2 = randBetween(.2f,  .4f);

    float type = randBetween(.0f, 3.0f);
    
    if (type < 1.0f) {
        return Quaternion::fromYawPitchRoll(axis1, axis2, 0);
    } else
    if (type < 2.0f) {
        return Quaternion::fromYawPitchRoll(axis2, axis1, axis2);
    }
    else {
        return Quaternion::fromYawPitchRoll(axis1, axis2, axis2);
    }
}
