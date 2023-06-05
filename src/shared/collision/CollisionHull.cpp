#include "CollisionShape.h"

#include "domain/Mesh.h"

#include <iostream>
#include "CollisionHull.h"

using namespace aa;


CollisionHull::CollisionHull(
	Object3d*	   parent,
	CollisionLayer objectLayer,
	int            collideableLayers,
	const std::vector <Vector3d>& points,
	float scale
) : CollisionShape(parent, objectLayer, collideableLayers, nullptr),
	points(points)
{
	for (auto& pt : this->points) {
		pt.x *= scale;
		pt.y *= scale;
		pt.z *= scale;
	}
}

CollisionHull::~CollisionHull()
{

}


bool CollisionHull::collidesWith(CollisionShape* other)
{
	return false;
}

