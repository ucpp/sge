#ifndef _SGE_APPLICATION_H_
#define _SGE_APPLICATION_H_

#include "sge_window.h"

namespace SGE
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

    private:
        void Initialize();
        void Update(double deltaTime);
        void Shutdown();

    private:
        Window* m_window;
    };
}

#endif // !_SGE_APPLICATION_H_