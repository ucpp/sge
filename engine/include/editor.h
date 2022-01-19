#pragma once

#include <memory>

#include "imgui_renderer.h"

namespace sge
{
    class RenderState
    {
    public:
        bool polygon_mode_enabled = false;
        bool normal_maps_enabled = true;
    };

    class Editor
    {
    public:
        void initialize(GLFWwindow *window, std::weak_ptr<Scene> scene, bool draw_gui);
        void update(float delta_time);
        void shutdown();

    private:
        bool draw_gui;
        RenderState state;
        ImGuiRenderer imgui_renderer;
    };
}