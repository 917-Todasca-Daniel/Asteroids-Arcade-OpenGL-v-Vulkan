#pragma once

#include "LogicObject.h"


namespace aa
{
    class Object3d : public LogicObject
    {

    public:
        Object3d(LogicObject *parent);
        virtual ~Object3d();

        virtual void draw() = 0;

        //  delete all implicit constructors 
        Object3d()                  = delete;
        Object3d(const Object3d&)   = delete;
        Object3d(Object3d&&)        = delete;

        Object3d& operator = (const Object3d&)  = delete;
        Object3d& operator = (Object3d&&)       = delete;


    private:

    };
}
