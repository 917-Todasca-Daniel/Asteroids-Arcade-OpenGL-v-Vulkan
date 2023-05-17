#include "Mesh.h"

using namespace aa;



Mesh::Mesh(LogicObject* parent, Vector3d position)
	: Object3d(parent, position), rotation(0, 0, 0, 0)
{

}

Mesh::~Mesh()
{

}
