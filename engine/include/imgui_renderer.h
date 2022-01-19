#pragma once

#include <memory>
#include <vector>

#include "scene.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace sge
{
    class RenderState;

    class ImGuiRenderer
    {
    public:
        void initialize(GLFWwindow *window, RenderState *state, std::weak_ptr<Scene> scene);
        void update(float delta_time);
        void shutdown();

    private:
        void drawTitle();
        void drawFPS(float delta_time);
        void drawPolygonModeSettings();
        void drawNormalMapsSettings();

    private:
        RenderState *state{nullptr};
        std::weak_ptr<Scene> scene;
        std::vector<std::string> objects;
        int selected_object;
    };
}