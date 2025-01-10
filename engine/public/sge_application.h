#ifndef _SGE_APPLICATION_H_
#define _SGE_APPLICATION_H_

#include <memory>
#include "sge_window.h"
#include "sge_scene.h"
#include "sge_renderer.h"
#include "sge_non_copyable.h"
#include "sge_render_context.h"
#include "sge_application_settings.h"

namespace SGE
{
    class Application : public NonCopyable
    {
    public:
        explicit Application(const std::string& configPath);
        void Run();

    private:
        void Initialize();
        void Update(double deltaTime);
        void Shutdown();

        void ResizeWindow(uint32 width, uint32 height);

    private:
        std::unique_ptr<Window> m_window;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<RenderContext> m_renderContext;
        std::unique_ptr<ApplicationSettings> m_settings;
        std::unique_ptr<Editor> m_editor;
        std::unique_ptr<Scene> m_scene;
    };
}

#endif // !_SGE_APPLICATION_H_