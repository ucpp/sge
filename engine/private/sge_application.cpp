#include "sge_application.h"

#include "sge_config.h"
#include "sge_editor.h"
#include "sge_frame_timer.h"
#include "sge_input.h"
#include "sge_logger.h"

namespace SGE
{
    Application::Application(const std::string& configPath)
    : m_appData(std::make_unique<ApplicationData>())
    , m_window(std::make_unique<Window>(m_appData.get()))
    , m_renderContext(std::make_unique<RenderContext>())
    , m_editor(std::make_unique<Editor>())
    , m_scene(std::make_unique<Scene>())
    , m_renderer(std::make_unique<Renderer>())
    , m_shaderMonitor(std::make_unique<DirectoryMonitor>(SHADERS_DIRECTORY))
    {
        bool configLoaded = Config::Load(configPath, *m_appData);
        Verify(configLoaded, "Failed to load settings");
    }

    void Application::Run()
    {
        Initialize();
        MainLoop();
        Shutdown();
    }

    void Application::Initialize()
    {
        FrameTimer timer{};
        
        m_window->Create();
        m_renderContext->Initialize(m_window.get(), m_appData.get());
        m_editor->Initialize(m_renderContext.get());
        m_scene->Initialize(m_renderContext.get());
        m_renderer->Initialize(m_renderContext.get());

        LOG_INFO("Initialization time: {}", timer.GetElapsedSeconds());

        m_shaderMonitor->OnDirectoryChanged().Subscribe(this, &Application::ShaderDirectoryChanged);
        m_shaderMonitor->Start();
    }

    void Application::MainLoop()
    {
        const double fixedDeltaTime = 1.0 / m_appData->windowData.targetFPS;
        FrameTimer timer{};
        m_isRunning = true;
        double accumulatedTime = 0.0;

        while (m_isRunning)
        {
            Input::Get().ResetStates();

            double elapsedTime = timer.GetElapsedSeconds();
            timer.Reset();

            accumulatedTime += elapsedTime;

            m_isRunning &= m_window->ProcessMessages();
            m_isRunning &= !m_appData->windowData.isPressedQuit;

            HandleInput();
            while (accumulatedTime >= fixedDeltaTime)
            {
                Update(fixedDeltaTime);
                accumulatedTime -= fixedDeltaTime;
            }

            Render();
        }
    }

    void Application::Update(double deltaTime)
    {
        m_scene->Update(deltaTime);
    }

    void Application::Render()
    {
        m_renderer->Render(m_scene.get(), m_editor.get());
    }

    void Application::HandleInput()
    {
        if(Input::Get().GetKeyDown(VK_ESCAPE))
        {
            m_isRunning = false;
        }

        if(Input::Get().GetKeyDown('Q'))
        {
            WindowData& windowData = m_renderContext->GetWindowData();
            windowData.isEditorEnable = !windowData.isEditorEnable;
            m_editor->SetActive(windowData.isEditorEnable);
        }
    }

    void Application::ShaderDirectoryChanged()
    {
        LOG_INFO("Shader directory changed, reloading shaders...");
        m_renderer->ReloadShaders();
    }

    void Application::Shutdown()
    {
        if(m_shaderMonitor)
        {
            m_shaderMonitor->OnDirectoryChanged().Unsubscribe(this);
            m_shaderMonitor->Stop();
        }

        if(m_scene)
        {
            m_scene->Shutdown();
        }

        if(m_editor)
        {
            m_editor->Shutdown();
        }

        if(m_renderer)
        {
            m_renderer->Shutdown();
        }

        if(m_renderContext)
        {
            m_renderContext->Shutdown();
        }
    }
}