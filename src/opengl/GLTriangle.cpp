#include "GLTriangle.h"

#include <iostream>

using namespace aa;


GLTriangle::GLTriangle(LogicObject* parent) : Object3d(parent)
{

}

GLTriangle::~GLTriangle()
{

}


void GLTriangle::draw()
{
	if (rmPending) return;
}
