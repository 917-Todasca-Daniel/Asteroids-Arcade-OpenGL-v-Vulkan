#include "Object3d.h"

#include <iostream>

using namespace aa;


Object3d::Object3d(LogicObject* parent, Vector3d position)
	: LogicObject(parent), position(position), bInit(false)
{

}

Object3d::~Object3d()
{

}


void Object3d::init() {
	if (bInit) {
		std::cout << this << " called init multiple times!\n";
	}
	bInit = true;
}

void Object3d::draw() const {
	if (!bInit) {
		std::cout << this << " did not call init!\n";
	}
}
