#pragma once

#include "Object3d.h"


namespace aa
{

    // forward declarations
    class Mesh;

    // logic class for the asteroids inside the game
    class Asteroid : public Object3d
    {

    public:
        Asteroid(
            LogicObject* parent, 
            Vector3d position,
            Mesh* mesh
        );
        virtual ~Asteroid();

        virtual void kill() override;

        virtual void init();

        virtual void loop(float lap) override;

        virtual void draw();


    protected:
        Mesh* asteroidMesh;

    };
}
