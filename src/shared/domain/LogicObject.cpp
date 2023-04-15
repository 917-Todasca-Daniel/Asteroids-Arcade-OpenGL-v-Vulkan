#include "LogicObject.h"

using namespace aa;


LogicObject::LogicObject(LogicObject* parent) : parent(parent), rmPending(false)
{
	
}

LogicObject::~LogicObject()
{

}



void aa::LogicObject::kill()
{
	this->rmPending = true;
}

void LogicObject::loop(float lap) 
{
	if (parent && parent->rmPending)
	{
		this->rmPending = true;
	}
}
