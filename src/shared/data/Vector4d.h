#pragma once

#include <ostream>


namespace aa
{
    //  inmutable data class for 4d vector
    class Vector4d
    {

    public:
        Vector4d(float x, float y, float z, float w);
        Vector4d(const Vector4d& other);

        Vector4d& operator = (const Vector4d&);

        float x;
        float y;
        float z;
        float w;

        //  delete all implicit constructors 
        Vector4d()                  = delete;
        Vector4d(Vector4d&&)        = delete;

        Vector4d& operator = (Vector4d&&)       = delete;

    };


    std::ostream& operator << (std::ostream& os, const Vector4d& v);

}
