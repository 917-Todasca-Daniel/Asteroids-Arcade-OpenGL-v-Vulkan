#pragma once


#include <cstdio>

#include "data/Vector4d.h"


namespace aa
{

    // colors are represented with Vector4d objects
    class UColors {

    public:
        static const Vector4d RED;
        static const Vector4d GREEN;
        static const Vector4d BLUE;
        static const Vector4d PINK;


        Vector4d fromRGB(int r, int g, int b);

        Vector4d fromRGBA(int r, int g, int b, float a);


        Vector4d fromHex(const char* hex);

        Vector4d fromHex8(const char* hex8);

    };

}
