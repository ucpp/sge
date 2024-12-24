#include "sge_application.h"
#include "sge_logger.h"

namespace SGE
{
    void Application::Run()
    {
        Initialize();
        Shutdown();
    }

    void Application::Initialize()
    {
        m_window = std::make_unique<Window>();
        m_window->Create("SGE: simple game engine v.0.9.0", 800, 600, false);
        m_window->OnUpdate().Subscribe(this, &Application::Update);

        m_renderer = std::make_unique<Renderer>();
        m_renderer->Initialize(m_window.get());

        m_window->StartUpdateLoop();
    }

    void Application::Update(double deltaTime)
    {
        m_renderer->Update(deltaTime);
        m_renderer->Render();
    }

    void Application::Shutdown()
    {
        m_window->OnUpdate().Unsubscribe(this);
        m_renderer->Shutdown();
        m_renderer.reset();
        m_window.reset();
    }
}