#include "Vector3d.h"

using namespace aa;


Vector3d::Vector3d(float x, float y, float z) : x(x), y(y), z(z)
{

}

Vector3d::Vector3d(const Vector3d& other) : x(other.x), y(other.y), z(other.z)
{

}


Vector3d& Vector3d::operator = (const Vector3d& other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
}

Vector3d Vector3d::operator - () const
{
    return Vector3d(-x, -y, -z);
}


std::ostream& aa::operator<<(std::ostream& os, const Vector3d& v)
{
    os << "V(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
