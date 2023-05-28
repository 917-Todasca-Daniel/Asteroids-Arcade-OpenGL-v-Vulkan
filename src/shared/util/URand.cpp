#include "URand.h"

#include <random>
#include <time.h>

#include "window_constants.hpp"

using namespace aa;


#define ROT_X_MIN       800.0f
#define ROT_X_MAX       1500.0f

#define Y_SCALE_LESS    0.8f
#define Y_SCALE_GREATER 1.3f


int URand::seed = time(0);


float URand::randBetween(float x, float y) {
    std::mt19937 rng(URand::seed);
    std::uniform_real_distribution<float> distribution(x, y);
    float randomNumber = distribution(rng);

    return randomNumber;
}


Vector3d URand::randAcceleration() {
    float x = randBetween(ROT_X_MIN, ROT_X_MAX);
    float y = randBetween(x * Y_SCALE_LESS, x * Y_SCALE_GREATER);

    return Vector3d(x, y, .0f);
}


Vector3d URand::randPosition() {
    float x = -WINDOW_WIDTH * 2.5f;
    float y = -WINDOW_HEIGHT * 4.0f;

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
