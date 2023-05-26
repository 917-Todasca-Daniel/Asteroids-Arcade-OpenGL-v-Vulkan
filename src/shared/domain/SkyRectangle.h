#pragma once

#include "domain/LogicObject.h"


namespace aa
{

    // forward declarations
    class Object3d;
    class GLShader;


    // logic class for the starry sky background (fills in the whole window)
    // composive relationship with necessary graphics object
    class SkyRectangle final : public LogicObject
    {

    public:
        SkyRectangle(
            LogicObject*    parent
        );
        virtual ~SkyRectangle();

        virtual void kill();

        virtual void loop(float lap);

        virtual void init();

        void draw();


    private:
        Object3d*       sky;

        const float     height;
        const float     width;

        float           lifespan;

    };
}
