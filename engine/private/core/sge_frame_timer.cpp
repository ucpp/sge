#include "core/sge_frame_timer.h"

namespace SGE
{
    FrameTimer::FrameTimer()
    {
        QueryPerformanceFrequency(&m_qpcFrequency);
        Reset();
    }

    void FrameTimer::Reset()
    {
        QueryPerformanceCounter(&m_qpcLastTime);
    }

    double FrameTimer::GetElapsedSeconds()
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        UINT64 timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;
        return static_cast<double>(timeDelta) / m_qpcFrequency.QuadPart;
    }
}