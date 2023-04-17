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


GLRectangle::GLRectangle(LogicObject* parent, Vector3d position, float height, float width)
	: GLRectangle(parent, position, height, width, UColors::PINK)
{

}

GLRectangle::GLRectangle(
	LogicObject*	parent, 
	Vector3d		position, 
	float			height,
	float			width,
	Vector4d		color
)
	: Object3d(parent, position), buffer(0), shader(0), ibo(0),
	height(height), width(width), color(color)
{

}

GLRectangle::~GLRectangle()
{
	glDeleteBuffers(1, &buffer);
	glDeleteBuffers(1, &ibo);

}


void GLRectangle::init()
{

	vertices2d[0] = 1.0f * (position.x - width /2) / WINDOW_UNIT;
	vertices2d[1] = 1.0f * (position.y - height/2) / WINDOW_UNIT;

	aa::UMaths::worldRectangleBottomLeft(
		height, width,
		vertices2d[0], vertices2d[1],
		vertices2d[2], vertices2d[3],
		vertices2d[4], vertices2d[5],
		vertices2d[6], vertices2d[7]
	);

	aa::UMaths::worldToGLCoord(vertices2d[0], vertices2d[1]);
	aa::UMaths::worldToGLCoord(vertices2d[2], vertices2d[3]);
	aa::UMaths::worldToGLCoord(vertices2d[4], vertices2d[5]);
	aa::UMaths::worldToGLCoord(vertices2d[6], vertices2d[7]);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices2d, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		6 * sizeof(unsigned int),
		indices, 
		GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	shader = GLShaders::readShader(
		"v_basic_position",
		"f_uniform_color"
	);

}


void GLRectangle::draw()
{
	Object3d::draw();

	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glUseProgram(shader);
	glUniform4f(
		glGetUniformLocation(
			shader, "u_color"
		),
		color.x, color.y, color.z, color.w
	);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}
