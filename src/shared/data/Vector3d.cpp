#include "Vector3d.h"

using namespace aa;


template<typename T>
Vector3d <T>::Vector3d(T x, T y, T z) : x(x), y(y), z(z)
{

}


template<typename T>
T Vector3d <T>::xx() const
{
	return this->x;
}

template<typename T>
T Vector3d <T>::yy() const
{
	return this->y;
}

template<typename T>
T Vector3d <T>::zz() const
{
	return this->z;
}
