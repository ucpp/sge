#include "sge_application.h"

#include "sge_logger.h"
#include "sge_frame_timer.h"
#include "sge_config.h"

namespace SGE
{
    Application::Application(const std::string& configPath)
    {
        m_settings = std::make_unique<ApplicationSettings>();
        if (!Config::Load(configPath, *m_settings))
        {
            LOG_ERROR("Failed to load settings from {}", configPath);
        }
    }

    void Application::Run()
    {
        Initialize();
        Shutdown();
    }

    void Application::Initialize()
    {
        FrameTimer timer;
    
        m_window = std::make_unique<Window>(m_settings.get());
        m_window->Create();
        m_window->OnUpdate().Subscribe(this, &Application::Update);

        m_renderer = std::make_unique<Renderer>();
        m_renderer->Initialize(m_window.get(), m_settings.get());

        LOG_INFO("Initialization time: {}", timer.GetElapsedSeconds());

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