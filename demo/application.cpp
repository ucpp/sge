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
    glfwInitHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window_ = glfwCreateWindow(width_, height_, kTitleWindow, nullptr, nullptr);
    if(window_ == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Falied create window");
    }

    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, &Application::ResizeCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }

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
    Engine::ResourceManager::LoadShader("shaders/default.vert", "shaders/default.frag", "default");

    glEnable(GL_DEPTH_TEST);

    float vertices[] = 
    {
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Engine::ResourceManager::LoadTexture("resources/1.jpg", "box", false);
    Engine::ResourceManager::LoadTexture("resources/2.png", "face", true);
}

void Application::Update()
{
    glm::vec3 cube_positions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    auto shader = Engine:: ResourceManager::GetShader("default");
    auto texture1 = Engine::ResourceManager::GetTexture("box");
    auto texture2 = Engine::ResourceManager::GetTexture("face");

    shader.Use();
    shader.SetInt("firstTexture", 0);
    shader.SetInt("secondTexture", 1);

    double delta_time = 0.0f;
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window_))
    {
        double current_time = glfwGetTime();
        delta_time = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        ProcessInput(window_);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        texture1.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.Bind();


        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
        glm::mat4 projection = glm::mat4(1.0f);
        float aspect = static_cast<float>(width_)/static_cast<float>(height_);
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        shader.Use();
 
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);

        glBindVertexArray(VAO_);
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);

            float angle = 20 * (i+1);
            model = glm::rotate(model, (float)current_time * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

            shader.SetMatrix4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        DrawGui();

        glfwSwapBuffers(window_);
    }
}

void Application::DrawGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), 1);
    ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::TextColored(ImColor(255, 255, 255, 255), "OpenGL render version 0.1");

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Application::Shutdown()
{
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
    
    ShutdownGui();

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

void Application::ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Application::ResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::ErrorCallback(int error_code, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}