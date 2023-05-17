#pragma once

#include "domain/Object3d.h"


namespace aa
{

    class VKPipeline;


    // draws a static 2d equilateral triangle to the screen;
    // the given position is the topmost corner's position on the screen relative to WINDOW_UNIT
    class VKTriangle : public Object3d
    {

    public:
        VKTriangle(
            LogicObject*    parent, 
            VKPipeline*     pipeline
        );
        virtual ~VKTriangle();

        virtual void init();

        virtual void draw();


    private:
        VKPipeline* pipeline;

    };
}
