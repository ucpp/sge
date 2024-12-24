#ifndef _SGE_WINDOW_H_
#define _SGE_WINDOW_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>
#include <string>

#include "sge_frame_timer.h"
#include "sge_action.h"

namespace SGE
{
    class Window
    {
    public:
        Window();
        virtual ~Window();

        void Create(const std::string& title, int width, int height, bool fullscreen);
        void StartUpdateLoop();

        void SetWindowSize(int width, int height);
        void SetFullscreen(bool fullscreen);

        inline HWND GetHandle() const { return m_hwnd; }
        inline int GetWidth() const { return m_width; }
        inline int GetHeight() const { return m_height; }
        inline bool IsFullscreen() const { return m_fullscreen; }

        Action<double>& OnUpdate() { return m_updateEvent; }

    private:
        void RegisterWindowClass();
        void CreateWindowHandle(int width, int height);
        void RetrieveClientSize();
        void EnableFullscreen();
        void DisableFullscreen();

        void DestroyWindowHandle();
        void UnregisterWindowClass();

        void Update();
        void Shutdown();

        static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

    private:
        LPCWSTR m_title;
        int m_width;
        int m_height;
        bool m_fullscreen;

        HINSTANCE m_hinstance;
        HWND m_hwnd;

        FrameTimer m_frameTimer;
        Action<double> m_updateEvent;
    };
}

#endif // !_SGE_WINDOW_H_