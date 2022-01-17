#pragma once

#include "input_system.h"
#include "renderer.h"
#include "imgui_renderer.h"
#include "config.h"
#include "scene.h"
#include "window.h"

namespace sge
{
    class Camera;
}

class Application
{
public:
    Application(std::string path_to_config);
    void run();

private:
    void initialize();
    void update();
    void shutdown();
    static void errorCallback(int error_code, const char *description);

private:
    sge::Window *window {nullptr};
    sge::Scene *scene {nullptr};
    sge::Renderer renderer;
    sge::InputSystem input;
    sge::RenderState state;
    sge::ImGuiRenderer imgui_renderer;
    sge::Config config;

    float delta_time;
};