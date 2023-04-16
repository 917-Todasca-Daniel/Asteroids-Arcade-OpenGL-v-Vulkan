#include "Vector4d.h"

using namespace aa;


Vector4d::Vector4d(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{

}

Vector4d::Vector4d(const Vector4d& other)
    : x(other.x), y(other.y), z(other.z), w(other.w)
{

}


std::ostream& operator<<(std::ostream& os, const Vector4d& v)
{
    os << "V(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}
