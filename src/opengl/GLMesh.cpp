#include "GLMesh.h"

#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//  assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>

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
	ibo(0), vbo(0), center(0, 0, 0)
{

}

GLMesh::~GLMesh()
{
	// make sure shader deletion is handled in a module above GLObject3d
}


void GLMesh::loadFromFbx(const char* filepath) {
	if (bInit) {
		std::cout << "GLObject3d::loadFromFbx should be called before ::init()!\n";
		return;
	}
	if (!vertices.empty()) {
		std::cout << "Trying to load an already loaded mesh!\n";
		return;
	}

	Assimp::Importer* importer = UFile::getAssimpFileReader();

	const aiScene* scene = importer->ReadFile(filepath, ASSIMP_LOAD_FLAGS);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading scene: " << importer->GetErrorString() << std::endl;
	}
	else {
		double centerX = 0, centerY = 0, centerZ = 0;
		unsigned int totalVertices = 0;

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
				const aiVector3D& pos = mesh->mVertices[j];
				const aiVector3D& normal = mesh->mNormals[j];
				const aiVector3D& texCoord = mesh->mTextureCoords[0][j];

				vertices.insert(
					vertices.end(), 
					{ 
						pos.x, pos.y, pos.z, normal.x, normal.y, normal.z,
						texCoord.y, texCoord.x
					}
				);

				center.x += pos.x, center.y += pos.y, center.z += pos.z;
				totalVertices += 1;
			}

			for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
				const aiFace& face = mesh->mFaces[j];

				for (unsigned int k = 0; k < face.mNumIndices; ++k) {
					indices.push_back(face.mIndices[k]);
				}
			}
		}

		center.x = float(centerX / totalVertices);
		center.y = float(centerY / totalVertices);
		center.z = float(centerZ / totalVertices);
	}

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

		if (rotation.w > 0) {
			projection *= Matrix4d::RotateAround(rotation, center);
		}

		shader->addUniformMat4f(
			"u_Projection",
			projection
		);
	}

	shader->bind();

	glDrawElements(GL_TRIANGLES, (unsigned int) indices.size(), GL_UNSIGNED_INT, 0);

}
