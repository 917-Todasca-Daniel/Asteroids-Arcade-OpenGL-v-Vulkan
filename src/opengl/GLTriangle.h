#pragma once

#include "domain/Object3d.h"


namespace aa
{
    // draws a static 2d equilateral triangle to the screen;
    // the given position is the topmost corner's position on the screen, 
    //      relative to the game viewport
    class GLTriangle : public Object3d
    {

    public:
        GLTriangle(LogicObject *parent, Vector3d position, float altitude);
        GLTriangle(
            LogicObject*    parent, 
            Vector3d        position, 
            float           edge, 
            Vector4d        color
        );
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
        const Vector4d color;
        const float altitude;

        unsigned int buffer;
        unsigned int shader;

        float vertices2d[6] = { };

    };
}
