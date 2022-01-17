#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "camera.h"
#include "window.h"
#include "scene.h"

Application::Application(std::string path_to_config)
{
    if (config.load(path_to_config))
    {
        scene = std::make_shared<sge::Scene>(config.data.getStartScene());
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
    auto settings = config.data.settings;
    window = std::make_shared<sge::Window>(&input, settings.window_width, settings.window_height, settings.application_name.c_str());

    renderer.initialize(settings.vsync_enabled ? 1 : 0);
    sge::ResourceManager::loadResources(config.data.resources);
    editor.initialize(window->get(), scene.get(), settings.imgui_enabled);
    scene->initialize(input);
}

void Application::update()
{
    double last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window->get()))
    {
        calculateDeltaTime(last_frame_time);
        glfwPollEvents();

        renderer.render();
        scene->update(delta_time, window->getWidth(), window->getHeight());
        editor.update(delta_time);

        glfwSwapBuffers(window->get());
    }
}

void Application::calculateDeltaTime(double& last_frame_time)
{
    double current_time = glfwGetTime();
    delta_time = current_time - last_frame_time;
    last_frame_time = current_time;
}

void Application::shutdown()
{
    editor.shutdown();
    scene->shutdown();
    window->shutdown();

    sge::ResourceManager::clear();
    glfwTerminate();
}