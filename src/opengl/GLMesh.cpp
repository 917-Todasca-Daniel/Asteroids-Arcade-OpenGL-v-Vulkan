#include "GLMesh.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//  assimp
#include <assimp/postprocess.h>

//	dev includes
#include "GLShaders.h"
#include "GLTexture.h"

#include "util/UFile.h"
#include "util/UColors.h"
#include "util/UMaths.h"
#include "constants/window_constants.hpp"

#include "data/Matrix4d.hpp"
#include "data/Quaternion.hpp"

using namespace aa;


#define ASSIMP_LOAD_FLAGS ( \
	aiProcess_Triangulate		| aiProcess_FlipUVs |				\
	aiProcess_GenSmoothNormals	| aiProcess_JoinIdenticalVertices	\
)



GLMesh::GLMesh(
	LogicObject*	parent, 
	Vector3d		position, 
	GLShader*		shader
)
	: Mesh(parent, position), shader(shader), 
	ibo(0), vbo(0)
{

}

GLMesh::~GLMesh()
{
	// make sure shader deletion is handled in a module above GLObject3d
}


void GLMesh::loadFromFbx(const char* filepath, float scale) {
	Mesh::loadFromFbx(filepath, ASSIMP_LOAD_FLAGS, scale);
}


void GLMesh::init()
{
	Object3d::init();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, 
		vertices.size() * sizeof(float),
		vertices.data(),
		GL_STATIC_DRAW
	);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		indices.size() * sizeof(unsigned int), 
		indices.data(), 
		GL_STATIC_DRAW
	);

}


void GLMesh::draw()
{
	Object3d::draw();

	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
		0
	);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
		(void*)(3 * sizeof(float))
	);
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float))
	);

	{	// fixed lightning and camera perspective
		shader->addUniform3f(
			"u_LightColor",
			Vector3d(1.0f,	1.0f,	1.0f)
		);
		shader->addUniform3f(
			"u_LightDirection",
			Vector3d(-1.0f, -1.0f, -1.0f)
		);
		shader->addUniform3f(
			"u_ViewPos",
			Vector3d(0.0f,	0.0f,	100.0)
		);
	}

	{	// translation, scaling and rotation geometry
		Matrix4d projection = Matrix4d::ViewportMatrix();
		projection *= Matrix4d::TranslationMatrix(position);

		projection *= Matrix4d::RotateAround(rotation, center);

		shader->addUniformMat4f(
			"u_Projection",
			projection
		);
	}

	shader->bind();

	glDrawElements(GL_TRIANGLES, (unsigned int) indices.size(), GL_UNSIGNED_INT, 0);

}
