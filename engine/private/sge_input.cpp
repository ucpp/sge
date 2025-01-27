#include "sge_input.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SGE
{
    void Input::ResetStates()
    {
        m_prevKeyStates = m_keyStates;
        m_prevMouseButtons = m_mouseButtons;
        m_mouseWheelDelta = 0;
    }

    LRESULT Input::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        if(ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam))
        {
            return 1;
        }
 
        switch (umsg)
        {
        case WM_KEYDOWN:
            KeyDown(static_cast<uint32>(wparam));
            return 0;
        case WM_KEYUP:
            KeyUp(static_cast<uint32>(wparam));
            return 0;
        case WM_MOUSEMOVE:
        {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);

            if (x >= 0 && x < clientRect.right && y >= 0 && y < clientRect.bottom)
            {
               MouseMove(x, y);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
            MouseButtonDown(MouseButton::Left);
            return 0;
        case WM_LBUTTONUP:
            MouseButtonUp(MouseButton::Left);
            return 0;
        case WM_RBUTTONDOWN:
            MouseButtonDown(MouseButton::Right);
            return 0;
        case WM_RBUTTONUP:
            MouseButtonUp(MouseButton::Right);
            return 0;
        case WM_MOUSEWHEEL:
            MouseWheel(GET_WHEEL_DELTA_WPARAM(wparam));
            return 0;
        default:
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}
