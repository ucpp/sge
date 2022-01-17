#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>

#include "input_system.h"

namespace sge
{
    Window::Window(InputSystem *input, uint32_t width, uint32_t height, const std::string& title) : width(width), height(height)
    {
        this->input = input;
        this->title = title.c_str();

        window = glfwCreateWindow(this->width, this->height, this->title, nullptr, nullptr);
        if (window == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Falied create window");
        }

        glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, &Window::resizeCallback);
        glfwSetKeyCallback(window, &Window::keyCallback);
        glfwSetCursorPosCallback(window, &Window::mouseCallback);
        glfwSetMouseButtonCallback(window, &Window::mouseButtonCallback);
        glfwSetScrollCallback(window, &Window::mouseScrollCallback);
    }

    GLFWwindow* Window::get() const
    {
        return this->window;
    }

    uint32_t Window::getWidth() const
    {
        return width;
    }

    uint32_t Window::getHeight() const
    {
        return height;
    }

    void Window::shutdown()
    {
        input = nullptr;

        glfwDestroyWindow(window);
        window = nullptr;
    }

    void Window::keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods)
    {
        Window *wnd = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        assert(wnd != nullptr);
        if(wnd->input == nullptr)
        {
            return;
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        wnd->input->processInput(key, action);
    }

    void Window::mouseCallback(GLFWwindow *window, double x, double y)
    {
        Window *wnd = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        assert(wnd != nullptr);
        if(wnd->input == nullptr)
        {
            return;
        }

        wnd->input->processMouseMovement(x, y);
    }

    void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        Window *wnd = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        assert(wnd != nullptr);
        if(wnd->input == nullptr)
        {
            return;
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            wnd->input->setPressedRightMouse(action == GLFW_PRESS);
        }
    }

    void Window::mouseScrollCallback(GLFWwindow *window, double x, double y)
    {
        Window *wnd = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        assert(wnd != nullptr);
        if(wnd->input == nullptr)
        {
            return;
        }

        wnd->input->processMouseScroll(y);
    }

    void Window::resizeCallback(GLFWwindow *window, int width, int height)
    {
        Window *wnd = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        assert(wnd != nullptr);

        glViewport(0, 0, width, height);
        wnd->width = width;
        wnd->height = height;
    }
}