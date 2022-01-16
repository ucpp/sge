#include "input_system.h"
#include <glfw/glfw3.h>

namespace Engine
{
    void InputSystem::ProcessMouseMovement(double x, double y)
    {
        last_mouse_x_ = x;
        last_mouse_y_ = y;
    }

    void InputSystem::ProcessMouseScroll(float offset)
    {
        on_mouse_scroll.Invoke(offset);
    }

    void InputSystem::SetPressedRightMouse(bool pressed)
    {
        mouse_right_button_pressed_ = pressed;
    }

    double InputSystem::GetX()
    {
        return last_mouse_x_;
    }

    double InputSystem::GetY()
    {
        return last_mouse_y_;
    }

    bool InputSystem::IsPressedMouseRight()
    {
        return mouse_right_button_pressed_;
    }

    void InputSystem::ProcessInput(int key, int action)
    {
        if (key >= 0 && key < 512)
        {
            key_state_[key] = action == GLFW_PRESS || action == GLFW_REPEAT ? 1 : 0;
        }
    }

    bool InputSystem::IsKeyPressed(int key_code)
    {
        if (key_code < 0 || key_code >= 512)
        {
            return false;
        }

        return key_state_[key_code] == GLFW_PRESS;
    }

    InputSystem::~InputSystem()
    {
        on_mouse_scroll.Clear();
    }
}