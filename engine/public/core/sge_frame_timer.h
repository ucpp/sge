#ifndef _SGE_FRAME_TIMER_H_
#define _SGE_FRAME_TIMER_H_

#include "pch.h"

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