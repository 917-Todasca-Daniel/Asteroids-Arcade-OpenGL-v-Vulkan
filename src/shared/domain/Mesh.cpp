#include "Mesh.h"

#include <iostream>

//  assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>

#include "util/UFile.h"

using namespace aa;



Mesh::Mesh(LogicObject* parent, Vector3d position)
	: Object3d(parent, position), rotation(0, 0, 0, 0),
	center(0, 0, 0)
{

}

Mesh::~Mesh()
{

}


void Mesh::loadFromFbx(const char* filepath, unsigned int pFlags) {
	if (bInit) {
		std::cout << "Mesh::loadFromFbx should be called before ::init()!\n";
		return;
	}
	if (!vertices.empty()) {
		std::cout << "Trying to load an already loaded mesh!\n";
		return;
	}

	Assimp::Importer* importer = UFile::getAssimpFileReader();

	const aiScene* scene = importer->ReadFile(filepath, pFlags);

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
