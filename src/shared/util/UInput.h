#pragma once


#include <cstdio>

#include "data/Vector4d.h"

struct GLFWwindow;


namespace aa
{

    // colors are represented with Vector4d objects
    class UInput {

    public:
        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


        static bool isLeftKeyPressed();

        static bool isRightKeyPressed();

        static bool isDownKeyPressed();

        static bool isUpKeyPressed();

        static bool isSpaceBarPressed();

    };

}
