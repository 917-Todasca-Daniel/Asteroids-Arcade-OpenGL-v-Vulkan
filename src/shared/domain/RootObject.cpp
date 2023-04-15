#include "RootObject.h"

using namespace aa;


//	static members must be declared - singleton instance is null
RootObject* RootObject::instance = nullptr;

RootObject::RootObject() : LogicObject(nullptr)
{

}

RootObject::~RootObject()
{

}


RootObject* RootObject::getRoot()
{
	if (instance == nullptr) {
		instance = new RootObject();
	}
	return instance;
}

void RootObject::init()
{

}

void RootObject::loop(float lap)
{

}
