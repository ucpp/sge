#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "camera.h"
#include "window.h"
#include "scene.h"
#include "log.h"

Application::Application(std::string path_to_config)
{
	bool loaded_config = config.load(path_to_config);
	assert(loaded_config && "Could not loaded config!");
}

void Application::run()
{
	initialize();
	update();
	shutdown();
}

void Application::initialize()
{
	scene = std::make_shared<sge::Scene>(config.data.getStartScene());
	auto settings = config.data.settings;
	window = std::make_shared<sge::Window>(input, settings.window_width, settings.window_height, settings.application_name.c_str());

	renderer.initialize(settings.vsync_enabled ? 1 : 0);
	renderer.setClearColor(glm::vec3(settings.clear_color.r, settings.clear_color.g, settings.clear_color.b));
	sge::ResourceManager::loadResources(config.data.resources);
	editor.initialize(std::weak_ptr(window), std::weak_ptr(scene), settings.imgui_enabled);
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
	scene = {};

	window->shutdown();
	window = {};

	sge::ResourceManager::clear();
	glfwTerminate();
}