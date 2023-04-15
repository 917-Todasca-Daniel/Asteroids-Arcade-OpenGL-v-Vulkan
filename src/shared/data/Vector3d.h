#pragma once


namespace aa
{
    //  inmutable data class for 3d vector
    template <typename T>
    class Vector3d
    {

    public:
        Vector3d(T x, T y, T z);

        T xx() const;
        T yy() const;
        T zz() const;

        //  delete all implicit constructors 
        Vector3d()                  = delete;
        Vector3d(const Vector3d&)   = delete;
        Vector3d(Vector3d&&)        = delete;

        Vector3d& operator = (const Vector3d&)  = delete;
        Vector3d& operator = (Vector3d&&)       = delete;


    private:
        T x;
        T y;
        T z;

    };
}
