#include "renderer.h"

#include "resource_manager.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sge
{
	Renderer::Renderer() : clear_color(glm::vec3(0.0f)) {}

	void Renderer::initialize(bool vsync)
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("Failed to initialize glad");
		}

		glfwSwapInterval(vsync);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void Renderer::render()
	{
		clearScreen();
	}

	void Renderer::setViewport(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::setClearColor(glm::vec3 color)
	{
		clear_color = color;
	}

	void Renderer::clearScreen()
	{
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
	}
}