#pragma once

#include "Object3d.h"


namespace aa
{

    // forward declarations
    class GLMesh;

    // logic class for the asteroids inside the game
    class Asteroid : public Object3d
    {

    public:
        Asteroid(
            LogicObject* parent, 
            Vector3d position,
            GLMesh* mesh
        );
        virtual ~Asteroid();

        virtual void kill() override;

        virtual void init();

        virtual void loop(float lap) override;


    protected:
        GLMesh* asteroidMesh;

    };
}
