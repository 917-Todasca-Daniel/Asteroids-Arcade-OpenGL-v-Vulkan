#pragma once

#include <ostream>


namespace aa
{
    //  inmutable data class for 3d vector
    class Vector4d
    {

    public:
        Vector4d(float x, float y, float z, float w);

        const float x;
        const float y;
        const float z;
        const float w;

        //  delete all implicit constructors 
        Vector4d()                  = delete;
        Vector4d(const Vector4d&)   = delete;
        Vector4d(Vector4d&&)        = delete;

        Vector4d& operator = (const Vector4d&)  = delete;
        Vector4d& operator = (Vector4d&&)       = delete;

    };


    std::ostream& operator << (std::ostream& os, const Vector4d& v);

}
