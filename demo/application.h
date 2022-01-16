#pragma once

#include "input_system.h"
#include "renderer.h"
#include "imgui_renderer.h"
#include "config.h"
#include "scene.h"

namespace sge
{
    class Camera;
}

class GLFWwindow;

class Application
{
public:
    Application(std::string path_to_config);
    void Run();

private:
    void Init();
    void InitGui();
    void Update();
    void DrawGui(float delta_time);
    void Shutdown();
    void ShutdownGui();

    static void KeyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void ErrorCallback(int error_code, const char *description);
    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void MouseScrollCallback(GLFWwindow *window, double x, double y);

private:
    GLFWwindow *window_{nullptr};
    sge::Scene *scene_{nullptr};

    sge::Renderer renderer_;
    sge::InputSystem input_;
    sge::RenderState state_;
    sge::ImGuiRenderer imgui_renderer_;
    sge::Config config_;

    float delta_time_;

    unsigned int width_;
    unsigned int height_;

    const char *kTitleWindow;
};