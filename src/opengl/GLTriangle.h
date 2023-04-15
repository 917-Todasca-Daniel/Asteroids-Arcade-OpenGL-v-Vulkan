#pragma once

#include "domain/Object3d.h"


namespace aa
{
    class GLTriangle : public Object3d
    {

    public:
        GLTriangle(LogicObject *parent);
        virtual ~GLTriangle();

        void draw();

        //  delete all implicit constructors 
        GLTriangle()                  = delete;
        GLTriangle(const GLTriangle&)   = delete;
        GLTriangle(GLTriangle&&)        = delete;

        GLTriangle& operator = (const GLTriangle&)  = delete;
        GLTriangle& operator = (GLTriangle&&)       = delete;


    private:

    };
}
