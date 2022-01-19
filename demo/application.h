#pragma once

#include "input_system.h"
#include "renderer.h"
#include "imgui_renderer.h"
#include "config.h"
#include "editor.h"

namespace sge
{
    class Camera;
    class Window;
    class Scene;
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
    void calculateDeltaTime(double& last_frame_time);

private:
    std::shared_ptr<sge::Window> window;
    std::shared_ptr<sge::Scene> scene;

    sge::Renderer renderer;
    sge::InputSystem input;
    sge::Config config;
    sge::Editor editor;

    double delta_time {0};
};