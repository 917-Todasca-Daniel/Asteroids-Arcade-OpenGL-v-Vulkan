#include "CollisionHull.h"

#include "CollisionSphere.h"

#include "domain/Mesh.h"
#include "util/UMaths.h"

#include <iostream>

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


void CollisionHull::updateAfterLoop() {
	if (framePoints.empty()) {
		framePoints = points;
	}

	for (int i=0; i<framePoints.size(); i++) {
		framePoints[i] = points[i];
		framePoints[i] = rotation.rotatePoint(framePoints[i]);
		framePoints[i] += position;
	}
}

bool CollisionHull::collidesWith(CollisionShape* other)
{
	bool ret = false;

	if ((collideableLayers & other->getCollisionLayer()) == 0) {
		ret = false;
	}
	else {
		auto sphere_cast = dynamic_cast<CollisionSphere*> (other);

		if (sphere_cast) {
			ret = false;
		}

		auto hull_cast = dynamic_cast<CollisionHull*> (other);
		if (hull_cast) {
			ret = UMaths::GJK(framePoints, hull_cast->framePoints);
		}
	}

	return ret;
}

