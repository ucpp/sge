#include "sge_window.h"

#include "sge_input.h"
#include "sge_logger.h"

namespace SGE
{
    Window::Window()
    : m_title(nullptr)
    , m_width(0)
    , m_height(0)
    , m_fullscreen(false)
    , m_hinstance(nullptr)
    , m_hwnd(nullptr) { }
    
    Window::~Window()
    {
        Shutdown();
    }
    
    void Window::Create(const std::string &title, int width, int height, bool fullscreen)
    {
        std::wstring wstrTitle = std::wstring(title.begin(), title.end());
        m_title = wstrTitle.c_str();

        m_fullscreen = fullscreen;

        RegisterWindowClass();
        CreateWindowHandle(width, height);
        RetrieveClientSize();
        SetFullscreen(fullscreen);
    }

    void Window::StartUpdateLoop()
    {
        Update();
    }

    void Window::SetWindowSize(int width, int height)
    {
        RECT windowRect = { 0, 0, width, height };
        AdjustWindowRectEx(&windowRect, GetWindowLong(m_hwnd, GWL_STYLE), false, GetWindowLong(m_hwnd, GWL_EXSTYLE));
        SetWindowPos(m_hwnd, HWND_NOTOPMOST, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
        m_width = width;
        m_height = height;
    }

    void Window::SetFullscreen(bool fullscreen)
    {
        if (m_fullscreen == fullscreen)
        {
            return;
        }
        
        if (fullscreen)
        {
            EnableFullscreen();
        }
        else
        {
            DisableFullscreen();
        }
    }
    
    void Window::RegisterWindowClass()
    {
        WNDCLASSEX wc {};
        m_hinstance = GetModuleHandle(nullptr);
        
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = MessageHandler;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_hinstance;
        wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = m_title;
        wc.cbSize = sizeof(WNDCLASSEX);

        RegisterClassEx(&wc);
    }

    void Window::CreateWindowHandle(int width, int height)
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        DWORD windowStyle = WS_OVERLAPPEDWINDOW;
        DWORD windowExStyle = WS_EX_APPWINDOW;
        RECT windowRect = {0, 0, width, height };
        AdjustWindowRectEx(&windowRect, windowStyle, false, windowExStyle);

        m_hwnd = CreateWindowEx(windowExStyle, m_title, m_title, WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, m_hinstance, this);
        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        SetFocus(m_hwnd);
    }

    void Window::RetrieveClientSize()
    {
        RECT windowRect;
        GetClientRect(m_hwnd, &windowRect);
        m_width = windowRect.right - windowRect.left;
        m_height = windowRect.bottom - windowRect.top;
    }

    void Window::EnableFullscreen()
    {
        DEVMODE dmScreenSettings = {};
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(m_width);
        dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(m_height);
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, m_width, m_height, SWP_FRAMECHANGED);
        ShowCursor(false);
        m_fullscreen = true;
    }

    void Window::DisableFullscreen()
    {
        ChangeDisplaySettings(nullptr, 0);
        SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_hwnd, HWND_NOTOPMOST, 0, 0, m_width, m_height, SWP_FRAMECHANGED);
        ShowCursor(true);
        m_fullscreen = false;
    }

    void Window::DestroyWindowHandle()
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    void Window::UnregisterWindowClass()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        UnregisterClass(m_title, hInstance);
    }

    void Window::Update()
    {
        MSG msg;
        bool done;

        ZeroMemory(&msg, sizeof(MSG));

        done = false;
        while (!done)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (msg.message == WM_QUIT || Input::Get().GetKeyDown(VK_ESCAPE))
            {
                done = true;
                break;
            }

            Input::Get().ResetStates();
        }
    }

    void Window::Shutdown()
    {
        DestroyWindowHandle();
        UnregisterWindowClass();
    }

    LRESULT CALLBACK Window::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        switch (umsg)
        {
        case WM_CREATE:
        {
            LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return Input::Get().MessageHandler(hwnd, umsg, wparam, lparam);
        }
        }
    }
}