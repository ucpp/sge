#pragma once

#include "scene.h"

class GLFWwindow;

namespace Engine
{
    class RenderState;

    class ImGuiRenderer
    {
    public:
        void Init(GLFWwindow *window, RenderState *state, Scene *scene);
        void Update(float delta_time);
        void Shutdown();

    private:
        void DrawTitle();
        void DrawFPS(float delta_time);
        void DrawPolygonModeSettings();
        void DrawNormalMapsSettings();

    private:
        RenderState *state_{nullptr};
        Scene *scene_{nullptr};
    };
}