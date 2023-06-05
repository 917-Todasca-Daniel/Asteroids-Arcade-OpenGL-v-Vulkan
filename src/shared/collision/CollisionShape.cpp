#include "CollisionShape.h"

#include "domain/Mesh.h"

#include <iostream>

using namespace aa;


CollisionShape::CollisionShape(
	Object3d*	   parent,
	CollisionLayer objectLayer,
	int            collideableLayers,
	Mesh*		   mesh
) : parent(parent), debugMesh(mesh), 
	objectLayer(objectLayer), collideableLayers(collideableLayers),
	rotation(0, 0, 0, 0), position(0, 0, 0)
{

}

CollisionShape::~CollisionShape()
{
	if (debugMesh) {
		delete debugMesh;
	}
}

