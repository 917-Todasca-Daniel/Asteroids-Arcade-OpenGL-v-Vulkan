#pragma once

#include "LogicObject.h"


#define AA_ROOT aa::RootObject::getRoot()

namespace aa
{
    //  daemon-like singleton for other game logic objects;
    //  if there's no root, then there is no game
    class RootObject final : public LogicObject
    {

    public:
        static RootObject* getRoot();

        virtual void loop(float lap);

        //  delete all implicit constructors 
        RootObject(const RootObject&)   = delete;
        RootObject(RootObject&&)        = delete;

        RootObject& operator = (const RootObject&)  = delete;
        RootObject& operator = (RootObject&&)       = delete;


    private:
        RootObject();

        //  private dtor means we do not support further inheritance
        virtual ~RootObject();

        static RootObject* instance;

    };
}
