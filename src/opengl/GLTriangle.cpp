#include "GLTriangle.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace aa;


GLTriangle::GLTriangle(LogicObject* parent) : Object3d(parent), buffer(0)
{

}

GLTriangle::~GLTriangle()
{

}


void GLTriangle::init()
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

}


void GLTriangle::draw()
{
	if (rmPending) return;

	glDrawArrays(GL_TRIANGLES, 0, 3);

}
