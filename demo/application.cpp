#include "application.h"

#include "resource_manager.h"

#include <stdexcept>
#include <iostream>
#include <math.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Application::Application(const unsigned int width, const unsigned int height, const char* title)
{
    width_ = width;
    height_ = height;

    kTitleWindow = title;
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

    if(!glfwInit())
    {
        throw std::runtime_error("Faild initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window_ = glfwCreateWindow(width_, height_, kTitleWindow, nullptr, nullptr);
    if(window_ == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Falied create window");
    }

    glfwSetWindowUserPointer(window_, reinterpret_cast<void*>(this));
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, &Application::ResizeCallback);
    glfwSetKeyCallback(window_, &Application::KeyCallback);
    glfwSetCursorPosCallback(window_, &Application::MouseCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }
    
    glfwSwapInterval(0);

    InitGui();
    InitRender();
}

void Application::InitGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);

    #ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 150");
    #else
    ImGui_ImplOpenGL3_Init("#version 130");
    #endif

    ImGui::StyleColorsDark();
}

//TODO: move to render
void Application::InitRender()
{
    glEnable(GL_DEPTH_TEST);

    Engine::ResourceManager::LoadShader("shaders/default.vert", "shaders/default.frag", "default");
    scene_.Load("resources/backpack/backpack.obj");
    camera_.Init(glm::vec3(0.0f, 0.0f, -3.0f));
}

void Application::Update()
{
    auto shader = Engine:: ResourceManager::GetShader("default");

    delta_time_ = 0.0f;
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window_))
    {
        double current_time = glfwGetTime();
        delta_time_ = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        ProcessInput(window_, delta_time_);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera_.GetViewMatrix();

        glm::mat4 projection = glm::mat4(1.0f);
        float aspect = static_cast<float>(width_)/static_cast<float>(height_);
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

        shader.Use();
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        shader.SetMatrix4("model", model);

        scene_.Draw(shader);

        DrawGui(delta_time_);

        glfwSwapBuffers(window_);
    }
}

void Application::DrawGui(float delta_time)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), 1);
    ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::TextColored(ImColor(255, 255, 255, 255), "OpenGL render version 0.1");
    ImGui::TextColored(ImColor(255, 255, 255, 255), (std::to_string((int)(1.0f/delta_time)) + " fps").c_str());

    ImGui::End();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Shutdown()
{
    ShutdownGui();

    scene_.Clear();
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

int i = 0;

void Application::KeyCallback(GLFWwindow* window, int key, int scan_code, int action, int mods)
{
    Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    //if(action == GLFW_PRESS)
    {
        application->camera_.ProcessInput(key, application->delta_time_);
    }
}

void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

	if (application->first_mouse)
	{
		application->last_x = xpos;
		application->last_y = ypos;
		application->first_mouse = false;
	}

	float x = xpos - application->last_x;
	float y = application->last_y - ypos;

	application->last_x = xpos;
	application->last_y = ypos;

	application->camera_.ProcessMouseMovement(x, y);
}

void Application::ProcessInput(GLFWwindow* window, float delta_time)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Application::ResizeCallback(GLFWwindow* window, int width, int height)
{
    Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    glViewport(0, 0, width, height);
}

void Application::ErrorCallback(int error_code, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}