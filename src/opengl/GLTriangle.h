#pragma once

#include "domain/Object3d.h"


namespace aa
{
    //  test object; draws a 2d triangle to the screen
    class GLTriangle : public Object3d
    {

    public:
        GLTriangle(LogicObject *parent);
        virtual ~GLTriangle();

        virtual void init();

        virtual void draw();

        //  delete all implicit constructors 
        GLTriangle()                  = delete;
        GLTriangle(const GLTriangle&)   = delete;
        GLTriangle(GLTriangle&&)        = delete;

        GLTriangle& operator = (const GLTriangle&)  = delete;
        GLTriangle& operator = (GLTriangle&&)       = delete;


    private:
        float positions[6] = {
            -0.5f, -0.5f,
             0.0f,  0.5f,
             0.5f, -0.5f
        };

        unsigned int buffer;

    };
}
