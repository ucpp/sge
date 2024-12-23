#include "sge_application.h"

namespace SGE
{
    void Application::Run()
    {
        Initialize();
    }

    void Application::Initialize()
    {
        m_window = std::make_unique<Window>();
        m_window->Create("SGE: simple game engine v.0.9.0", 800, 600, false);
        m_window->StartUpdateLoop();
    }

    void Application::Update(double deltaTime)
    {

    }

    void Application::Shutdown()
    {

    }
}