#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "model.h"
#include "camera.h"

class Application
{
public:
    Application(const unsigned int width, const unsigned int height, const char* title);
    void Run();

private:
    void Init();
    void InitRender();
    void InitGui();
    void Update();
    void DrawGui(float delta_time);
    void Shutdown();
    void ShutdownGui();

    void ProcessInput(GLFWwindow* window, float delta_time);
    static void KeyCallback(GLFWwindow* window, int key, int scan_code, int action, int mods);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void ErrorCallback(int error_code, const char* description);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
private:
    GLFWwindow* window_ {nullptr};
    unsigned int width_;
    unsigned int height_;
    unsigned int texture_;

    Engine::Model box_;
    Engine::Model light_;
    Engine::Camera camera_;
    Engine::Shader active_shader_;
    float delta_time_;
    bool mouse_pressed_;

    const char* kTitleWindow;

    float last_x = 0.0f;
    float last_y = 0.0f;

    bool polygon_mode_enabled_ = true;
    bool normal_maps_enabled_ = true;
};

#endif