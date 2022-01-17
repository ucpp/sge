#pragma once

#include <cstdint>
#include <string>

class GLFWwindow;

namespace sge
{
    class InputSystem;

    class Window
    {
    public:
        Window(InputSystem *input, uint32_t width, uint32_t height, const std::string& title);
        GLFWwindow* get() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        void shutdown();

    private:
        void initGlfw();
        void setApiProfile();
        void create();
        void addListeners();

        static void errorCallback(int error_code, const char *description);
        static void keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);
        static void resizeCallback(GLFWwindow *window, int width, int height);
        static void mouseCallback(GLFWwindow *window, double x, double y);
        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void mouseScrollCallback(GLFWwindow *window, double x, double y);

    private:
        GLFWwindow *window {nullptr};
        InputSystem *input {nullptr};
        uint32_t width {0};
        uint32_t height {0};

        const char *title;
    };
}