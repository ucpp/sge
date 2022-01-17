#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <math.h>

#include "resource_manager.h"
#include "camera.h"
#include "log.h"

Application::Application(std::string path_to_config)
{
    if (config.load(path_to_config))
    {
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
    
    auto settings = config.data.settings;
    window = new sge::Window(&input, settings.window_width, settings.window_height, settings.application_name.c_str());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }

    glfwSwapInterval(config.data.settings.vsync_enabled ? 1 : 0);

    imgui_renderer.initialize(window->get(), &state, scene);
    sge::ResourceManager::loadResources(config.data.resources);
    scene->initialize(input);
    renderer.initialize();
}

void Application::update()
{
    delta_time = 0.0f;
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window->get()))
    {
        double current_time = glfwGetTime();
        delta_time = current_time - last_frame_time;
        last_frame_time = current_time;
        glfwPollEvents();

        renderer.render();

        scene->update(delta_time, window->getWidth(), window->getHeight());

        if (config.data.settings.imgui_enabled)
        {
            imgui_renderer.update(delta_time);
        }

        glfwSwapBuffers(window->get());
    }
}

void Application::shutdown()
{
    imgui_renderer.shutdown();
    delete scene;
    scene = nullptr;

    sge::ResourceManager::clear();

    window->shutdown();
    delete window;
    window = nullptr;

    glfwTerminate();
}

void Application::errorCallback(int error_code, const char *description)
{
    sge::Log::error("Error: %s\n", description);
}