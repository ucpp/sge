#include "input_system.h"
#include <glfw/glfw3.h>

namespace sge
{
    InputSystem::InputSystem() : mouse_right_button_pressed(false)
    {}

    void InputSystem::processMouseMovement(double x, double y)
    {
        last_mouse_x = x;
        last_mouse_y = y;
    }

    void InputSystem::processMouseScroll(float offset)
    {
        on_mouse_scroll.invoke(offset);
    }

    void InputSystem::setPressedRightMouse(bool pressed)
    {
        mouse_right_button_pressed = pressed;
    }

    double InputSystem::getX()
    {
        return last_mouse_x;
    }

    double InputSystem::getY()
    {
        return last_mouse_y;
    }

    bool InputSystem::isPressedMouseRight()
    {
        return mouse_right_button_pressed;
    }

    void InputSystem::processInput(int key, int action)
    {
        if (key >= 0 && key < 512)
        {
            key_state[key] = action == GLFW_PRESS || action == GLFW_REPEAT ? 1 : 0;
        }
    }

    bool InputSystem::isKeyPressed(int key_code)
    {
        if (key_code < 0 || key_code >= 512)
        {
            return false;
        }

        return key_state[key_code] == GLFW_PRESS;
    }

    InputSystem::~InputSystem()
    {
        on_mouse_scroll.clear();
    }
}