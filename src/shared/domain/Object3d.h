#pragma once

#include "LogicObject.h"

#include "data/Vector3d.h"
#include "data/Vector4d.h"


namespace aa
{
    class Object3d : public LogicObject
    {

    public:
        Object3d(LogicObject *parent, Vector3d position);
        virtual ~Object3d();

        virtual void init();

        virtual void draw();

        // getters and setter
        void setPosition(Vector3d other) {
            this->position = other;
        }

        //  delete all implicit constructors 
        Object3d()                  = delete;
        Object3d(const Object3d&)   = delete;
        Object3d(Object3d&&)        = delete;

        Object3d& operator = (const Object3d&)  = delete;
        Object3d& operator = (Object3d&&)       = delete;


    protected:
        Vector3d position;

        bool     bInit;

    };
}
