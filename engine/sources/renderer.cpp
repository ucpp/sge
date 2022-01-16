#include "renderer.h"

#include "resource_manager.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sge
{
    void Renderer::initialize()
    {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void Renderer::render()
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    }

    void Renderer::setViewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}