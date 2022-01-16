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
    if (config.load(path_to_config))
    {
        window_width = config.data.settings.window_width;
        window_height = config.data.settings.window_height;
        title_window = config.data.settings.application_name.c_str();
        scene = new sge::Scene(config.data.getStartScene());
    }
}

void Application::run()
{
    initialize();
    update();
    shutdown();
}

void Application::initialize()
{
    glfwSetErrorCallback(&Application::errorCallback);

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

    window = glfwCreateWindow(window_width, window_height, title_window, nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Falied create window");
    }

    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, &Application::resizeCallback);
    glfwSetKeyCallback(window, &Application::keyCallback);
    glfwSetCursorPosCallback(window, &Application::mouseCallback);
    glfwSetMouseButtonCallback(window, &Application::mouseButtonCallback);
    glfwSetScrollCallback(window, &Application::mouseScrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }

    glfwSwapInterval(config.data.settings.vsync_enabled ? 1 : 0);

    imgui_renderer.initialize(window, &state, scene);
    sge::ResourceManager::loadResources(config.data.resources);
    scene->initialize(input);
    renderer.initialize();
}

void Application::update()
{
    delta_time = 0.0f;
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        double current_time = glfwGetTime();
        delta_time = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        renderer.render();

        scene->update(delta_time, window_width, window_height);

        if (config.data.settings.imgui_enabled)
        {
            imgui_renderer.update(delta_time);
        }

        glfwSwapBuffers(window);
    }
}

void Application::shutdown()
{
    imgui_renderer.shutdown();

    delete scene;
    scene = nullptr;

    sge::ResourceManager::clear();

    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();
}

void Application::keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    application->input.processInput(key, action);
}

void Application::mouseCallback(GLFWwindow *window, double x, double y)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    application->input.processMouseMovement(x, y);
}

void Application::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        application->input.setPressedRightMouse(action == GLFW_PRESS);
    }
}

void Application::mouseScrollCallback(GLFWwindow *window, double x, double y)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    application->input.processMouseScroll(y);
}

void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
    Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    assert(application != nullptr);

    glViewport(0, 0, width, height);
    application->window_width = width;
    application->window_height = height;
}

void Application::errorCallback(int error_code, const char *description)
{
    std::cerr << "Error: " << description << std::endl;
}