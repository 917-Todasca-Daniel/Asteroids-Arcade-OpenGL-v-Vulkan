#pragma once

#include "domain/Object3d.h"


namespace aa
{

    class GLShader;


    // draws a static 2d rectangle to the screen;
    // the given position is the center of the rectangle relative to the WINDOW_UNIT
    // expected shader: position, texture coord
    class GLRectangle : public Object3d
    {

    public:
        GLRectangle(
            LogicObject*    parent,
            Vector3d        position,
            float           height,
            float           width,
            GLShader*       shader
        );
        virtual ~GLRectangle();

        virtual void init();

        virtual void draw() const;

        //  delete all implicit constructors 
        GLRectangle()                   = delete;
        GLRectangle(const GLRectangle&) = delete;
        GLRectangle(GLRectangle&&)      = delete;

        GLRectangle& operator = (const GLRectangle&)    = delete;
        GLRectangle& operator = (GLRectangle&&)         = delete;


    private:
        const GLShader*  shader;

        const float      height;
        const float      width;

        unsigned int     buffer;
        unsigned int     ibo;

        //  4 tuples of (x_screen, y_screen, x_texture, y_texture)
        float vertices2d[16] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

    };
}
