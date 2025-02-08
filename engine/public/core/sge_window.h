#ifndef _SGE_WINDOW_H_
#define _SGE_WINDOW_H_

#include "pch.h"

#include "sge_frame_timer.h"
#include "data/sge_data_structures.h"
#include "sge_action.h"

namespace SGE
{
    class Window
    {
    public:
        Window(const class ApplicationData* appData);
        virtual ~Window();

        void Create();
        void Create(const std::string& title, int32 width, int32 height, bool fullscreen);
        bool ProcessMessages();

        void SetWindowSize(int32 width, int32 height);
        void SetFullscreen(bool fullscreen);

        inline HWND GetHandle() const { return m_hwnd; }
        inline int32 GetWidth() const { return m_width; }
        inline int32 GetHeight() const { return m_height; }
        inline bool IsFullscreen() const { return m_fullscreen; }

        Action<uint32, uint32>& OnResize() { return m_resizeEvent; }

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

        void SaveWindowPositionAndMonitor();
        void RestoreWindowPositionAndSize();
        void RestoreWindowPositionAndSizeFromFullscreen();

        static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

    private:
        LPCWSTR        m_title;
        int32          m_width;
        int32          m_height;
        bool           m_fullscreen;
        HINSTANCE      m_hinstance;
        HWND           m_hwnd;
        Action<uint32, uint32> m_resizeEvent;
    
        const class ApplicationData* m_appData;

        RECT           m_windowPosition;
        RECT           m_monitorRect;
    };
}

#endif // !_SGE_WINDOW_H_