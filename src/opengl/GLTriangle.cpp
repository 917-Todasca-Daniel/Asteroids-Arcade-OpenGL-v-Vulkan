#include "GLTriangle.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	dev includes
#include "util/UColors.hpp"
#include "util/UMaths.hpp"
#include "constants/window_constants.hpp"
#include "GLShaders.h"

using namespace aa;


GLTriangle::GLTriangle(LogicObject* parent, Vector3d position, float edge) 
	: GLTriangle(parent, position, edge, U_PINK)
{

}

GLTriangle::GLTriangle(
	LogicObject*	parent, 
	Vector3d		position, 
	float			altitude,
	Vector4d		color
)
	: Object3d(parent, position), buffer(0), altitude(altitude), color(color)
{

}

GLTriangle::~GLTriangle()
{
	glDeleteBuffers(1, &buffer);

}


void GLTriangle::init()
{
	vertices2d[0] = 1.0f * position.x / WINDOW_UNIT;
	vertices2d[1] = 1.0f * position.y / WINDOW_UNIT;

	U_worldTriangleTopAltitude(
		altitude,
		vertices2d[0], vertices2d[1],
		vertices2d[2], vertices2d[3],
		vertices2d[4], vertices2d[5]
	);

	U_worldToGLCoord(vertices2d[0], vertices2d[1]);
	U_worldToGLCoord(vertices2d[2], vertices2d[3]);
	U_worldToGLCoord(vertices2d[4], vertices2d[5]);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices2d, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	shader = GLShaders::readShader(
		"v_basic_position",
		"f_uniform_color"
	);
}


void GLTriangle::draw()
{
	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glUseProgram(shader);
	glUniform4f(
		glGetUniformLocation(
			shader, "u_color"
		),
		color.x, color.y, color.z, color.w
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);

}
