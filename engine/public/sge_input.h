#ifndef _SGE_INPUT_H_
#define _SGE_INPUT_H_

#include <array>
#include "sge_singleton.h"
#include "sge_window.h"

namespace SGE
{
    enum class MouseButton
    {
        Left,
        Right
    };

    class Input : public Singleton<Input>
    {
        friend class Singleton<Input>;
        friend class Window;

    public:
        inline bool GetKey(unsigned int keyCode) const { return m_keyStates[keyCode]; }
        inline bool GetKeyDown(unsigned int keyCode) const { return m_keyStates[keyCode] && !m_prevKeyStates[keyCode]; }
        inline bool GetKeyUp(unsigned int keyCode) const { return !m_keyStates[keyCode] && m_prevKeyStates[keyCode]; }

        inline bool GetMouseButton(unsigned int button) const { return m_mouseButtons[button]; }
        inline bool GetMouseButtonDown(MouseButton button) const { return m_mouseButtons[static_cast<int>(button)] && !m_prevMouseButtons[static_cast<int>(button)]; }
        inline bool GetMouseButtonUp(MouseButton button) const { return !m_mouseButtons[static_cast<int>(button)] && m_prevMouseButtons[static_cast<int>(button)]; }

        inline int GetMouseX() const { return m_mouseX; }
        inline int GetMouseY() const { return m_mouseY; }
        inline int GetMouseWheelDelta() const { return m_mouseWheelDelta; }

    private:
        LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

        inline void KeyDown(unsigned int keyCode) { m_keyStates[keyCode] = true; }
        inline void KeyUp(unsigned int keyCode) { m_keyStates[keyCode] = false; }

        inline void MouseMove(int xPos, int yPos) { m_mouseX = xPos; m_mouseY = yPos; }
        inline void MouseButtonDown(MouseButton button) { m_mouseButtons[static_cast<int>(button)] = true; }
        inline void MouseButtonUp(MouseButton button) { m_mouseButtons[static_cast<int>(button)] = false; }
        inline void MouseWheel(int delta) { m_mouseWheelDelta += delta; }

        void ResetStates();

    private:
        int m_mouseX = 0;
        int m_mouseY = 0;
        int m_mouseWheelDelta = 0;

        std::array<bool, 256> m_keyStates = { false };
        std::array<bool, 256> m_prevKeyStates = { false };
        std::array<bool, 2> m_mouseButtons = { false };
        std::array<bool, 2> m_prevMouseButtons = { false };
    };
}

#endif // !_SGE_INPUT_H_
