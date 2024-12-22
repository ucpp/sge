#include "sge_application.h"

namespace SGE
{
    Application::Application() : m_window(new Window()) {}

    Application::~Application()
    {
        delete m_window;
    }

    void Application::Run()
    {
        Initialize();
    }

    void Application::Initialize()
    {
        m_window->Create("Test", 800, 600, false);
        m_window->StartUpdateLoop();
    }

    void Application::Update(double deltaTime)
    {

    }

    void Application::Shutdown()
    {

    }
}