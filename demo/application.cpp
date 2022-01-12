#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <math.h>

#include "resource_manager.h"
#include "camera.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Application::Application(std::string path_to_config)
{
    if(config_.Load(path_to_config))
    {
        width_ = config_.data.settings.window_width;
        height_ = config_.data.settings.window_height;
        kTitleWindow = config_.data.settings.application_name.c_str();
        scene_ = new Engine::Scene(config_.data.GetStartScene());
    }
}

void Application::Run()
{
    Init();
    Update();
    Shutdown();
}

void Application::Init()
{
    glfwSetErrorCallback(&Application::ErrorCallback);

    if (!glfwInit())
    {
        throw std::runtime_error("Faild initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(width_, height_, kTitleWindow, nullptr, nullptr);
    if (window_ == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Falied create window");
    }

    glfwSetWindowUserPointer(window_, reinterpret_cast<void *>(this));
    glfwMakeContextCurrent(window_);

    glfwSetFramebufferSizeCallback(window_, &Application::ResizeCallback);
    glfwSetKeyCallback(window_, &Application::KeyCallback);

    glfwSetCursorPosCallback(window_, &Application::MouseCallback);
    glfwSetMouseButtonCallback(window_, &Application::MouseButtonCallback);
    glfwSetScrollCallback(window_, &Application::MouseScrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }

    glfwSwapInterval(config_.data.settings.vsync_enabled ? 1 : 0);

    imgui_renderer_.Init(window_, &state_, scene_);
    InitRender();
}

// TODO: move to render
void Application::InitRender()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Engine::ResourceManager::LoadResources(config_.data.resources);

    scene_->Init(input_);
}

void Application::Update()
{
    delta_time_ = 0.0f;
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window_))
    {
        double current_time = glfwGetTime();
        delta_time_ = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        ProcessInput(window_, delta_time_);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = static_cast<float>(width_) / static_cast<float>(height_);
        scene_->Update(delta_time_, aspect);

        if(config_.data.settings.imgui_enabled)
        {
            imgui_renderer_.Update(delta_time_);
        }

        glfwSwapBuffers(window_);
    }
}

void Application::Shutdown()
{
    imgui_renderer_.Shutdown();

    delete scene_;
    scene_ = nullptr;

    Engine::ResourceManager::Clear();

    glfwDestroyWindow(window_);
    window_ = nullptr;

    glfwTerminate();
}

void Application::ShutdownGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::KeyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    application->input_.ProcessInput(key, action);
}

void Application::MouseCallback(GLFWwindow *window, double x, double y)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    application->input_.ProcessMouseMovement(x, y);
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        application->input_.SetPressedRightMouse(action == GLFW_PRESS);
    }
}

void Application::MouseScrollCallback(GLFWwindow* window, double x, double y)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    application->input_.ProcessMouseScroll(y);
}

void Application::ProcessInput(GLFWwindow *window, float delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Application::ResizeCallback(GLFWwindow *window, int width, int height)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    glViewport(0, 0, width, height);
    application->width_ = width;
    application->height_ = height;
}

void Application::ErrorCallback(int error_code, const char *description)
{
    std::cerr << "Error: " << description << std::endl;
}