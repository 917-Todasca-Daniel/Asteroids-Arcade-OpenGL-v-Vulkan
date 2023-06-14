#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "UInput.h"

#include <iostream>

using namespace aa;


enum USER_KEYS {
    LEFT_KEY = 0, RIGHT_KEY, UP_KEY, DOWN_KEY, SPACE_KEY, USER_KEYS_COUNT
};


bool isKeyPressed[USER_KEYS_COUNT];


void UInput::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_LEFT:
                isKeyPressed[LEFT_KEY]  = true;
                break;
            case GLFW_KEY_RIGHT:
                isKeyPressed[RIGHT_KEY] = true;
                break;
            case GLFW_KEY_UP:
                isKeyPressed[UP_KEY]    = true;
                break;
            case GLFW_KEY_DOWN:
                isKeyPressed[DOWN_KEY]  = true;
                break;
            case GLFW_KEY_SPACE:
                isKeyPressed[SPACE_KEY] = true;
                break;
        }
    } else if(action == GLFW_RELEASE) {
        switch(key) {
            case GLFW_KEY_LEFT:
                isKeyPressed[LEFT_KEY]  = false;
                break;
            case GLFW_KEY_RIGHT:
                isKeyPressed[RIGHT_KEY] = false;
                break;
            case GLFW_KEY_UP:
                isKeyPressed[UP_KEY]    = false;
                break;
            case GLFW_KEY_DOWN:
                isKeyPressed[DOWN_KEY]  = false;
                break;
            case GLFW_KEY_SPACE:
                isKeyPressed[SPACE_KEY] = false;
                break;
        }
    }
}

bool UInput::isLeftKeyPressed() {
    return isKeyPressed[LEFT_KEY];
}

bool UInput::isRightKeyPressed() {
    return isKeyPressed[RIGHT_KEY];
}

bool UInput::isUpKeyPressed() {
    return isKeyPressed[UP_KEY];
}

bool UInput::isDownKeyPressed() {
    return isKeyPressed[DOWN_KEY];
}

bool UInput::isSpaceBarPressed() {
    return isKeyPressed[SPACE_KEY];
}
