#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <math.h>

#include "resource_manager.h"
#include "camera.h"
#include "model.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Application::Application(const unsigned int width, const unsigned int height, const char *title)
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }

    glfwSwapInterval(0);

    imgui_renderer_.Init(window_, &state_);
    InitRender();
}

// TODO: move to render
void Application::InitRender()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    // load shaders
    Engine::ResourceManager::LoadShader("shaders/lamp.vert", "shaders/lamp.frag", "lamp");
    Engine::ResourceManager::LoadShader("shaders/lighting.vert", "shaders/lighting.frag", "lighting");
    Engine::ResourceManager::LoadShader("shaders/basic_lighting.vert", "shaders/basic_lighting.frag", "basic");

    // load models
    Engine::ResourceManager::LoadModel("resources/box/box.obj", "light");
    Engine::ResourceManager::LoadModel("resources/pbr_sponza/sponza.obj", "sponza");

    //init camera
    main_camera_ = new Engine::Camera(input_, glm::vec3(0.0, 2.0, -3.0), 30.0);
}

void Application::Update()
{
    state_.active_shader = &Engine::ResourceManager::GetShader("lighting");
    auto lamp_shader = Engine::ResourceManager::GetShader("lamp");
    auto sponza_model = Engine::ResourceManager::GetModel("sponza");
    auto light_model = Engine::ResourceManager::GetModel("light");

    delta_time_ = 0.0f;
    double last_frame_time = 0.0f;


    //Test shadows
    /*
    const unsigned int shadow_map_width = 1024;
    const unsigned int shadow_map_height = 1024;
    unsigned int depth_map_FBO;
    glGenFramebuffers(1, &depth_map_FBO);

    unsigned int depth_map;
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_width, shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */


    while (!glfwWindowShouldClose(window_))
    {
        double current_time = glfwGetTime();
        delta_time_ = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        main_camera_->Update(delta_time_);
        ProcessInput(window_, delta_time_);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = main_camera_->GetViewMatrix();

        // TODO: move to init and resize
        glm::mat4 projection = glm::mat4(1.0f);
        float aspect = static_cast<float>(width_) / static_cast<float>(height_);
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

        const float radius = 6.0f;
        float dx = sin(current_time * 2) * radius;
        float dy = cos(current_time * 2) * radius;

        glm::vec3 lamp_pos = glm::vec3(dx, 5.0f, dy);
        glm::mat4 lamp_model = glm::mat4(1.0f);
        lamp_model = glm::translate(lamp_model, lamp_pos);
        lamp_model = glm::scale(lamp_model, glm::vec3(0.1f, 0.1f, 0.1f));

        state_.active_shader->Use();
        state_.active_shader->SetFloat("material.shininess", 128.0f);
        state_.active_shader->SetVec3("viewPosition", main_camera_->GetPosition());
        
        state_.active_shader->SetVec3("pointLights[0].color", 1.0f, 1.0f, 1.0f);
        state_.active_shader->SetVec3("pointLights[0].position", lamp_pos);

        state_.active_shader->SetFloat("pointLights[0].linear", 0.09f);
        state_.active_shader->SetFloat("pointLights[0].quadratic", 0.032f);

        state_.active_shader->SetFloat("pointLights[0].ambient", 0.5f);
        state_.active_shader->SetFloat("pointLights[0].diffuse", 0.7f);
        state_.active_shader->SetFloat("pointLights[0].specular", 1.0f);

        state_.active_shader->SetVec3("directionalLight.direction", -1.0f, -5.0f, -1.5f);
        state_.active_shader->SetFloat("directionalLight.ambient", 0.3f);
        state_.active_shader->SetFloat("directionalLight.diffuse", 0.8f);
        state_.active_shader->SetFloat("directionalLight.specular", 0.7f);

        state_.active_shader->SetMatrix4("view", view);
        state_.active_shader->SetMatrix4("projection", projection);
        state_.active_shader->SetMatrix4("model", model);

        sponza_model.Draw(*state_.active_shader);

        lamp_shader.Use();
        lamp_shader.SetMatrix4("view", view);
        lamp_shader.SetMatrix4("projection", projection);
        lamp_shader.SetMatrix4("model", lamp_model);

        light_model.Draw(lamp_shader);

        imgui_renderer_.Update(delta_time_);

        glfwSwapBuffers(window_);
    }
}

void Application::Shutdown()
{
    imgui_renderer_.Shutdown();
    
    delete main_camera_;
    main_camera_ = nullptr;

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
}

void Application::ErrorCallback(int error_code, const char *description)
{
    std::cerr << "Error: " << description << std::endl;
}