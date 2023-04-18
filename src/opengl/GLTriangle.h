#pragma once

#include "domain/Object3d.h"


namespace aa
{

    class GLShader;


    // draws a static 2d equilateral triangle to the screen;
    // the given position is the topmost corner's position on the screen relative to WINDOW_UNIT
    // expected shader: position
    class GLTriangle : public Object3d
    {

    public:
        GLTriangle(
            LogicObject*    parent, 
            Vector3d        position, 
            float           edge, 
            GLShader*       shader
        );
        virtual ~GLTriangle();

        virtual void init();

        virtual void draw() const;

        //  delete all implicit constructors 
        GLTriangle()                  = delete;
        GLTriangle(const GLTriangle&)   = delete;
        GLTriangle(GLTriangle&&)        = delete;

        GLTriangle& operator = (const GLTriangle&)  = delete;
        GLTriangle& operator = (GLTriangle&&)       = delete;


    private:
        const GLShader* shader;
        const float     altitude;
        unsigned int    buffer;

        float vertices2d[6] = { };

    };
}
