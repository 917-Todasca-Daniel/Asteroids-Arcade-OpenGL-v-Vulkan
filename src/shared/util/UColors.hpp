#pragma once


#include <cstdio>

#include "data/Vector4d.h"


namespace aa
{
    Vector4d fromRGB(int r, int g, int b) {
        float rx = static_cast<float>(r) / 255.0f;
        float gx = static_cast<float>(g) / 255.0f;
        float bx = static_cast<float>(b) / 255.0f;

        return Vector4d(rx, gx, bx, 1.0);
    }


    Vector4d fromRGBA(int r, int g, int b, float a) {
        float rx = static_cast<float>(r) / 255.0f;
        float gx = static_cast<float>(g) / 255.0f;
        float bx = static_cast<float>(b) / 255.0f;

        return Vector4d(rx, gx, bx, a);
    }

    Vector4d fromHex(const char *hex) {
        unsigned int colorValue = 0;
        if (std::sscanf(&hex[1], "%x", &colorValue) == EOF) {
            colorValue = -1;
        }
        int r = (colorValue >> 16) & 0xff;
        int g = (colorValue >> 8) & 0xff;
        int b = colorValue & 0xff;

        return aa::fromRGB(r, g, b);
    }


    Vector4d fromHex8(const char* hex8) {
        unsigned int colorValue = 0;
        if (std::sscanf(&hex8[1], "%x", &colorValue) == EOF) {
            colorValue = -1;
        }
        char r = (colorValue >> 24) & 0xff;
        char g = (colorValue >> 16) & 0xff;
        char b = (colorValue >> 8) & 0xff;

        float a = static_cast<float>(colorValue & 0xff) / 255.0f;
        return fromRGBA(r, g, b, a);
    }
}
