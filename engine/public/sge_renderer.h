#ifndef _SGE_RENDERER_H_
#define _SGE_RENDERER_H_

#include <memory>
#include "sge_non_copyable.h"
#include "sge_device.h"
#include "sge_window.h"

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(Window* window);
        void Update(double deltaTime);
        void Render();
        void Shutdown();

    private:
        std::unique_ptr<Device> m_device;
        Window* m_window = nullptr;
    };
}

#endif // !_SGE_RENDERER_H_