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
	: Object3d(parent, position), shader(shader), 
	ibo(0), vbo(0)
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
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
				const aiVector3D& pos = mesh->mVertices[j];
				const aiVector3D& normal = mesh->mNormals[j];

				vertices.insert(
					vertices.end(), 
					{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z }
				);
			}

			for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
				const aiFace& face = mesh->mFaces[j];

				for (unsigned int k = 0; k < face.mNumIndices; ++k) {
					indices.push_back(face.mIndices[k]);
				}
			}
		}
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


void GLMesh::draw() const
{
	Object3d::draw();

	if (rmPending) return;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		0
	);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*)(3 * sizeof(float))
	);

	shader->bind();

	glDrawElements(GL_TRIANGLES, (unsigned int) indices.size(), GL_UNSIGNED_INT, 0);

}
