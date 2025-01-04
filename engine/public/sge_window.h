#ifndef _SGE_WINDOW_H_
#define _SGE_WINDOW_H_

#include "pch.h"

#include "sge_frame_timer.h"
#include "sge_action.h"

namespace SGE
{
    class Window
    {
    public:
        Window(const struct ApplicationSettings* applicationSettings);
        virtual ~Window();

        void Create();
        void Create(const std::string& title, int32 width, int32 height, bool fullscreen);
        void StartUpdateLoop();

        void SetWindowSize(int32 width, int32 height);
        void SetFullscreen(bool fullscreen);

        inline HWND GetHandle() const { return m_hwnd; }
        inline int32 GetWidth() const { return m_width; }
        inline int32 GetHeight() const { return m_height; }
        inline bool IsFullscreen() const { return m_fullscreen; }

        Action<double>& OnUpdate() { return m_updateEvent; }

    private:
        void RegisterWindowClass();
        void CreateWindowHandle(int32 width, int32 height);
        void RetrieveClientSize();
        void EnableFullscreen();
        void DisableFullscreen();

        void DestroyWindowHandle();
        void UnregisterWindowClass();

        void Update();
        void Shutdown();

        static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

    private:
        LPCWSTR        m_title;
        int32          m_width;
        int32          m_height;
        bool           m_fullscreen;
        HINSTANCE      m_hinstance;
        HWND           m_hwnd;
        FrameTimer     m_frameTimer;
        Action<double> m_updateEvent;
    
        const struct ApplicationSettings* m_applicationSettings;
    };
}

#endif // !_SGE_WINDOW_H_