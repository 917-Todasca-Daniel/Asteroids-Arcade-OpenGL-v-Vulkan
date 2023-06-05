#pragma once

#include <ostream>


namespace aa
{
    //  inmutable data class for 3d vector
    class Vector3d
    {

    public:
        Vector3d(float x, float y, float z);
        Vector3d(const Vector3d& other);

        Vector3d& operator = (const Vector3d&);

        Vector3d operator - () const;

        Vector3d operator - (const Vector3d&) const;

        // scalar multiplication
        Vector3d operator * (float scalar) const;

        // += operator
        Vector3d& operator += (const Vector3d&);

        float x;
        float y;
        float z;

        //  delete all implicit constructors 
        Vector3d()                        = delete;

    };


    std::ostream& operator << (std::ostream& os, const Vector3d& v);

}
