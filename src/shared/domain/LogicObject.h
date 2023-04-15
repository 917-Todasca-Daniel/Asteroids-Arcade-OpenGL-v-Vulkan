#pragma once


namespace aa
{
    //  base logic object for the game;
    //  logic objects are looped every frame
    class LogicObject
    {

    public:
        LogicObject(LogicObject* parent = nullptr);
        virtual ~LogicObject();

        virtual void kill();

        virtual void init() = 0;

        virtual void loop(float lap);

        //  delete all implicit constructors 
        LogicObject()                   = delete;
        LogicObject(const LogicObject&) = delete;
        LogicObject(LogicObject&&)      = delete;

        LogicObject& operator = (const LogicObject&)    = delete;
        LogicObject& operator = (LogicObject&&)         = delete;


    protected:
        LogicObject* parent;

        bool rmPending;

    };
}
