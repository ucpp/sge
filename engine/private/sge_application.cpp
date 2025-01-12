#include "sge_application.h"

#include "sge_logger.h"
#include "sge_frame_timer.h"
#include "sge_config.h"
#include "sge_editor.h"

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
        FrameTimer timer{};
    
        m_window = std::make_unique<Window>(m_settings.get());
        m_window->Create();
        m_window->OnUpdate().Subscribe(this, &Application::Update);

        m_renderContext = std::make_unique<RenderContext>();
        m_renderContext->Initialize(m_window.get(), m_settings.get());

        m_editor = std::make_unique<Editor>();
        m_editor->Initialize(m_renderContext.get());

        m_scene = std::make_unique<Scene>();
        m_scene->Initialize(m_renderContext.get());

        m_renderer = std::make_unique<Renderer>();
        m_renderer->Initialize(m_renderContext.get());

        LOG_INFO("Initialization time: {}", timer.GetElapsedSeconds());

        m_window->StartUpdateLoop();
    }

    void Application::Update(double deltaTime)
    {
        m_scene->Update(deltaTime);
        m_renderer->Render(m_scene.get(), m_editor.get());
    }

    void Application::Shutdown()
    {
        if(m_scene)
        {
            m_scene->Shutdown();
            m_scene.reset();
        }

        if(m_editor)
        {
            m_editor->Shutdown();
            m_editor.reset();
        }
        
        if(m_window)
        {
            m_window->OnUpdate().Unsubscribe(this);
            m_window.reset();
        }

        if(m_renderer)
        {
            m_renderer->Shutdown();
            m_renderer.reset();
        }
        
        if(m_renderContext)
        {
            m_renderContext->Shutdown();
            m_renderContext.reset();
        }
    }
}