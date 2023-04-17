#pragma once

#include "domain/Object3d.h"


namespace aa
{
    // draws a 2d rectangle to the screen;
    // the given position is the center of the rectangle
    //      relative to the WINDOW_UNIT
    class GLRectangle : public Object3d
    {

    public:
        GLRectangle(
            LogicObject*    parent, 
            Vector3d        position, 
            float           height, 
            float           width
        );
        GLRectangle(
            LogicObject*    parent,
            Vector3d        position,
            float           height,
            float           width,
            Vector4d        color
        );
        virtual ~GLRectangle();

        virtual void init();

        virtual void draw();

        //  delete all implicit constructors 
        GLRectangle()                   = delete;
        GLRectangle(const GLRectangle&) = delete;
        GLRectangle(GLRectangle&&)      = delete;

        GLRectangle& operator = (const GLRectangle&)    = delete;
        GLRectangle& operator = (GLRectangle&&)         = delete;


    private:
        const Vector4d color;
        const float height;
        const float width;

        unsigned int buffer;
        unsigned int shader;
        unsigned int ibo;

        float vertices2d[8] = { };
        unsigned int indices[6] = { };

    };
}
