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
        inline bool GetKey(uint32 keyCode) const { return m_keyStates[keyCode]; }
        inline bool GetKeyDown(uint32 keyCode) const { return m_keyStates[keyCode] && !m_prevKeyStates[keyCode]; }
        inline bool GetKeyUp(uint32 keyCode) const { return !m_keyStates[keyCode] && m_prevKeyStates[keyCode]; }

        inline bool GetMouseButton(uint32 button) const { return m_mouseButtons[button]; }
        inline bool GetMouseButtonDown(MouseButton button) const { return m_mouseButtons[static_cast<int32>(button)] && !m_prevMouseButtons[static_cast<int32>(button)]; }
        inline bool GetMouseButtonUp(MouseButton button) const { return !m_mouseButtons[static_cast<int32>(button)] && m_prevMouseButtons[static_cast<int32>(button)]; }

        inline int32 GetMouseX() const { return m_mouseX; }
        inline int32 GetMouseY() const { return m_mouseY; }
        inline int32 GetMouseWheelDelta() const { return m_mouseWheelDelta; }

    private:
        LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

        inline void KeyDown(uint32 keyCode) { m_keyStates[keyCode] = true; }
        inline void KeyUp(uint32 keyCode) { m_keyStates[keyCode] = false; }

        inline void MouseMove(int32 xPos, int32 yPos) { m_mouseX = xPos; m_mouseY = yPos; }
        inline void MouseButtonDown(MouseButton button) { m_mouseButtons[static_cast<int32>(button)] = true; }
        inline void MouseButtonUp(MouseButton button) { m_mouseButtons[static_cast<int32>(button)] = false; }
        inline void MouseWheel(int32 delta) { m_mouseWheelDelta += delta; }

        void ResetStates();

    private:
        int32 m_mouseX = 0;
        int32 m_mouseY = 0;
        int32 m_mouseWheelDelta = 0;

        std::array<bool, 256> m_keyStates = { false };
        std::array<bool, 256> m_prevKeyStates = { false };
        std::array<bool, 2> m_mouseButtons = { false };
        std::array<bool, 2> m_prevMouseButtons = { false };
    };
}

#endif // !_SGE_INPUT_H_
