#include "CollisionSphere.h"

#include <iostream>

using namespace aa;


CollisionSphere::CollisionSphere(
	Object3d*	   parent, 
	CollisionLayer layer,
	int			   collideableLayers,
	float		   radius
)	: CollisionShape(parent, layer, collideableLayers, nullptr),
	radius(radius)
{

}

CollisionSphere::~CollisionSphere()
{

}


bool CollisionSphere::collidesWith(CollisionShape* other) {
	bool ret = false;
	
	if ((collideableLayers & other->getCollisionLayer()) == 0) {
		ret = false;
	}
	else {
		auto sphere_cast = dynamic_cast<CollisionSphere*> (other);

		if (sphere_cast) {
			auto diff = this->position - sphere_cast->position;
			double squaredDistance = (double)diff.x * diff.x +
				(double)diff.y * diff.y +
				(double)diff.z * diff.z;

			double spheresRadii = (double)this->radius + sphere_cast->radius;

			ret = (squaredDistance <= spheresRadii * spheresRadii);
		}
	}

	return ret;
}

