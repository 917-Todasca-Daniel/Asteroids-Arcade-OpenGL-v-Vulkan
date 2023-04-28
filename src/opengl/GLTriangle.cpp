#include "GLTriangle.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	dev includes
#include "util/UColors.h"
#include "util/UMaths.h"

#include "constants/window_constants.hpp"

#include "GLShaders.h"

using namespace aa;



GLTriangle::GLTriangle(
	LogicObject*	parent, 
	Vector3d		position, 
	float			altitude,
	GLShader*		shader
)
	: Object3d(parent, position), buffer(0),
	altitude(altitude), shader(shader)
{

}

GLTriangle::~GLTriangle()
{
	glDeleteBuffers(1, &buffer);
	
	// make sure shader is deleted in a module above
}


void GLTriangle::init()
{
	Object3d::init();

	vertices2d[0] = 1.0f * position.x / WINDOW_UNIT;
	vertices2d[1] = 1.0f * position.y / WINDOW_UNIT;

	aa::UMaths::worldTriangleTopAltitude(
		altitude,
		vertices2d[0], vertices2d[1],
		vertices2d[2], vertices2d[3],
		vertices2d[4], vertices2d[5]
	);

	aa::UMaths::worldToGLCoord(vertices2d[0], vertices2d[1]);
	aa::UMaths::worldToGLCoord(vertices2d[2], vertices2d[3]);
	aa::UMaths::worldToGLCoord(vertices2d[4], vertices2d[5]);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices2d, GL_STATIC_DRAW);

}


void GLTriangle::draw()
{
	Object3d::draw();

	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	
	shader->bind();

	glDrawArrays(GL_TRIANGLES, 0, 3);

}
