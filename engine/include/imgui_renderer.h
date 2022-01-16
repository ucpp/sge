#pragma once

#include "scene.h"

class GLFWwindow;

namespace sge
{
    class RenderState;

    class ImGuiRenderer
    {
    public:
        void initialize(GLFWwindow *window, RenderState *state, Scene *scene);
        void update(float delta_time);
        void shutdown();

    private:
        void drawTitle();
        void drawFPS(float delta_time);
        void drawPolygonModeSettings();
        void drawNormalMapsSettings();

    private:
        RenderState *state{nullptr};
        Scene *scene{nullptr};
    };
}