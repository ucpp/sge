#include "sge_window.h"

#include "sge_application_settings.h"
#include "sge_input.h"
#include "sge_logger.h"

namespace SGE
{
    Window::Window(const ApplicationSettings* applicationSettings)
    : m_title(nullptr)
    , m_width(0)
    , m_height(0)
    , m_fullscreen(false)
    , m_hinstance(nullptr)
    , m_hwnd(nullptr)
    , m_applicationSettings(applicationSettings) { }
    
    Window::~Window()
    {
        Shutdown();
    }

    void Window::Create()
    {
        if(m_applicationSettings == nullptr)
        {
            LOG_ERROR("Failed to create the window: Application settings are null.");
            return;
        }

        const std::string& title = m_applicationSettings->title;
        const uint32 width = m_applicationSettings->width;
        const uint32 height = m_applicationSettings->height;
        const bool fullscreen = m_applicationSettings->fullscreen;

        Create(title, width, height, fullscreen);
    }
    
    void Window::Create(const std::string& title, int32 width, int32 height, bool fullscreen)
    {
        std::wstring wstrTitle = std::wstring(title.begin(), title.end());
        m_title = wstrTitle.c_str();

        m_fullscreen = fullscreen;

        if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        {
            LOG_ERROR("Failed to set DPI awareness.");
        }

        RegisterWindowClass();
        CreateWindowHandle(width, height);
        RetrieveClientSize();
        SetFullscreen(fullscreen);

        LOG_INFO("Window created with parameters: title = {}, width = {}, height = {}", title, width, height);
    }

    void Window::StartUpdateLoop()
    {
        Update();
    }

    void Window::SetWindowSize(int32 width, int32 height)
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        if (width == m_width && height == m_height)
        {
            LOG_WARN("The window size will not be changed.");
            return;
        }

        SaveWindowPositionAndMonitor();

        RECT windowRect = { 0, 0, width, height };
        AdjustWindowRectEx(&windowRect, GetWindowLong(m_hwnd, GWL_STYLE), false, GetWindowLong(m_hwnd, GWL_EXSTYLE));
        int32 cx = windowRect.right - windowRect.left;
        int32 cy = windowRect.bottom - windowRect.top;

        SetWindowPos(m_hwnd, HWND_NOTOPMOST, m_windowPosition.left, m_windowPosition.top, cx, cy, SWP_NOZORDER | SWP_FRAMECHANGED);
        RetrieveClientSize();

        m_resizeEvent.Invoke(m_width, m_height);
    }

    void Window::SetFullscreen(bool fullscreen)
    {
        if (m_fullscreen == fullscreen)
        {
            return;
        }
        
        if (fullscreen)
        {
            SaveWindowPositionAndMonitor();
            EnableFullscreen();
        }
        else
        {
            DisableFullscreen();
            RestoreWindowPositionAndSize();
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

    void Window::CreateWindowHandle(int32 width, int32 height)
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
        DWORD windowExStyle = WS_EX_APPWINDOW;

        RECT windowRect = { 0, 0, width, height };

        AdjustWindowRectEx(&windowRect, windowStyle, false, windowExStyle);

        m_hwnd = CreateWindowEx(windowExStyle, m_title, m_title, windowStyle, 0, 0, 
                                windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 
                                nullptr, nullptr, m_hinstance, this);

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        SetFocus(m_hwnd);

        RetrieveClientSize();
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

        LOG_INFO("Fullscreen mode enabled.");
    }

    void Window::DisableFullscreen()
    {
        ChangeDisplaySettings(nullptr, 0);
        SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_hwnd, HWND_NOTOPMOST, 0, 0, m_width, m_height, SWP_FRAMECHANGED);
        ShowCursor(true);
        m_fullscreen = false;

        LOG_INFO("Fullscreen mode disabled.");
    }

    void Window::DestroyWindowHandle()
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;

        LOG_INFO("Destroyed window successfully.");
    }

    void Window::UnregisterWindowClass()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        UnregisterClass(m_title, hInstance);

        LOG_INFO("Window class unregistered successfully.");
    }

    void Window::Update()
    {
        m_frameTimer.Reset();

        MSG msg;
        bool done = false;

        ZeroMemory(&msg, sizeof(MSG));

        double accumulatedTime = 0.0f;
        const double fixedDeltaTime = 60.0f / 1000.0f;

        while (!done)
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                bool isPressedQuit = m_applicationSettings->isPressedQuit || Input::Get().GetKeyDown(VK_ESCAPE);
                if (msg.message == WM_QUIT || isPressedQuit)
                {
                    done = true;
                    break;
                }
            }

            if (!done)
            {
                double elapsedTime = m_frameTimer.GetElapsedSeconds();
                accumulatedTime += elapsedTime;

                while (accumulatedTime >= fixedDeltaTime)
                {
                    m_updateEvent.Invoke(fixedDeltaTime);
                    accumulatedTime -= fixedDeltaTime;
                }

                Input::Get().ResetStates();
                m_frameTimer.Reset();
            }
        }
    }

    void Window::Shutdown()
    {
        m_updateEvent.Clear();
        m_resizeEvent.Clear();

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
        case WM_KEYDOWN:
        if (wparam == VK_RETURN && (GetKeyState(VK_MENU) & 0x8000))
        {
            return 0;
        }
        default:
        {
            return Input::Get().MessageHandler(hwnd, umsg, wparam, lparam);
        }
        }
    }

    void Window::SaveWindowPositionAndMonitor()
    {
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        HMONITOR monitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        GetMonitorInfo(monitor, &monitorInfo);

        m_monitorRect = monitorInfo.rcMonitor;

        RECT windowRect;
        GetWindowRect(m_hwnd, &windowRect);
        m_windowPosition = windowRect;
    }

    void Window::RestoreWindowPositionAndSize()
    {
        if (m_windowPosition.left != 0 && m_windowPosition.top != 0)
        {
            SetWindowPos(m_hwnd, HWND_NOTOPMOST, m_windowPosition.left, m_windowPosition.top,
                         m_windowPosition.right - m_windowPosition.left, 
                         m_windowPosition.bottom - m_windowPosition.top, 
                         SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }

    void Window::RestoreWindowPositionAndSizeFromFullscreen()
    {
        if (!m_fullscreen)
        {
            RestoreWindowPositionAndSize();
        }
    }
}