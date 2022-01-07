#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

namespace Engine
{
    class InputSystem
    {
    public:
        void ProcessMouseMovement(double x, double y); 
        void SetPressedRightMouse(bool pressed);
        void ProcessInput(int key, int action);

        double GetX();
        double GetY();
        bool IsPressedMouseRight();
        bool IsKeyPressed(int key_code);

    private:
        double last_mouse_x_;
        double last_mouse_y_;
        bool mouse_right_button_pressed_;
        bool key_state_[512] {0};
    };
}

#endif