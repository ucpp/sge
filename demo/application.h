#ifndef APPLICATION_H
#define APPLICATION_H

#include "input_system.h"
#include "renderer.h"
#include "imgui_renderer.h"

namespace Engine
{
    class Camera;
}

class GLFWwindow;

class Application
{
public:
    Application(const unsigned int width, const unsigned int height, const char *title);
    void Run();

private:
    void Init();
    void InitRender();
    void InitGui();
    void Update();
    void DrawGui(float delta_time);
    void Shutdown();
    void ShutdownGui();

    void ProcessInput(GLFWwindow *window, float delta_time);
    static void KeyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void ErrorCallback(int error_code, const char *description);
    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void MouseScrollCallback(GLFWwindow *window, double x, double y);

private:
    GLFWwindow *window_{nullptr};
    Engine::Camera *main_camera_{nullptr};

    Engine::InputSystem input_;
    Engine::RenderState state_;
    Engine::ImGuiRenderer imgui_renderer_;

    unsigned int width_;
    unsigned int height_;
    unsigned int texture_;

    float delta_time_;

    const char *kTitleWindow;
};

#endif