#include "GLRectangle.h"

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	dev includes
#include "util/UColors.h"
#include "util/UMaths.h"

#include "constants/window_constants.hpp"

#include "GLShaders.h"

using namespace aa;



GLRectangle::GLRectangle(
	LogicObject*	parent, 
	Vector3d		position, 
	float			height,
	float			width,
	GLShader*		shader
)
	: Object3d(parent, position), buffer(0), ibo(0),
	height(height), width(width), shader(shader)
{

}

GLRectangle::~GLRectangle()
{
	glDeleteBuffers(1, &buffer);
	glDeleteBuffers(1, &ibo);

	// shader not deleted, as we allow for shader to be reused by other GL objects
	// make sure shader deletion is handled in a module above GLRectangle
}


void GLRectangle::init()
{
	Object3d::init();

	{	// optimise by using CPU instead of a shader for calculating coords
		vertices2d[0] = 1.0f * (position.x - width / 2) / WINDOW_UNIT;
		vertices2d[1] = 1.0f * (position.y - height / 2) / WINDOW_UNIT;

		aa::UMaths::worldRectangleBottomLeft(
			height, width,
			vertices2d[0],  vertices2d[1],
			vertices2d[4],  vertices2d[5],
			vertices2d[8],  vertices2d[9],
			vertices2d[12], vertices2d[13]
		);

		aa::UMaths::worldToGLCoord(vertices2d[0], vertices2d[1]);
		aa::UMaths::worldToGLCoord(vertices2d[4], vertices2d[5]);
		aa::UMaths::worldToGLCoord(vertices2d[8], vertices2d[9]);
		aa::UMaths::worldToGLCoord(vertices2d[12], vertices2d[13]);
	}

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices2d, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

}


void GLRectangle::draw() const
{
	Object3d::draw();

	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 
		0
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
		(void*)(sizeof(float) * 2)
	);

	shader->bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}
