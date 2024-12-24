#ifndef _SGE_FRAME_TIMER_H_
#define _SGE_FRAME_TIMER_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>

namespace SGE
{
    class FrameTimer
    {
    public:
        FrameTimer();
        void Reset();
        double GetElapsedSeconds();

    private:
        LARGE_INTEGER m_qpcFrequency;
        LARGE_INTEGER m_qpcLastTime;
    };
}

#endif // !_SGE_FRAME_TIMER_H_