#include "Vector3d.h"

using namespace aa;


Vector3d::Vector3d(float x, float y, float z) : x(x), y(y), z(z)
{

}


std::ostream& aa::operator<<(std::ostream& os, const Vector3d& v)
{
    os << "V(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
