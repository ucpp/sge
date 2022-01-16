#pragma once

#include "event.h"

namespace sge
{
    class InputSystem
    {
    public:
        ~InputSystem();

        void ProcessMouseMovement(double x, double y);
        void ProcessMouseScroll(float offset);
        void SetPressedRightMouse(bool pressed);
        void ProcessInput(int key, int action);

        double GetX();
        double GetY();
        double GetScroll();
        bool IsPressedMouseRight();
        bool IsKeyPressed(int key_code);

    public:
        Event<float> on_mouse_scroll;

    private:
        double last_mouse_x_;
        double last_mouse_y_;
        double last_scroll_offset_;
        bool mouse_right_button_pressed_;
        bool key_state_[512]{0};
    };
}