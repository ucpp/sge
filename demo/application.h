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
    void run();

private:
    void initialize();
    void initializeGui();

    void update();
    void drawGui(float delta_time);

    void shutdown();
    void shutdownGui();

    static void keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);
    static void resizeCallback(GLFWwindow *window, int width, int height);
    static void errorCallback(int error_code, const char *description);
    static void mouseCallback(GLFWwindow *window, double x, double y);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void mouseScrollCallback(GLFWwindow *window, double x, double y);

private:
    GLFWwindow *window{nullptr};
    
    sge::Scene *scene{nullptr};
    sge::Renderer renderer;
    sge::InputSystem input;
    sge::RenderState state;
    sge::ImGuiRenderer imgui_renderer;
    sge::Config config;

    float delta_time;

    uint32_t window_width;
    uint32_t window_height;

    const char *title_window;
};