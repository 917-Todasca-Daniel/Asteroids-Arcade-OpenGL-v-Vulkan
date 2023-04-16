#include "Object3d.h"

using namespace aa;


Object3d::Object3d(LogicObject* parent, Vector3d position) 
	: LogicObject(parent), position(position)
{

}

Object3d::~Object3d()
{

}
