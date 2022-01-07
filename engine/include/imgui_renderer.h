#ifndef IMGUI_RENDERER_H
#define IMGUI_RENDERER_H

class GLFWwindow;

namespace Engine
{
    class RenderState;

    class ImGuiRenderer
    {
    public:
        void Init(GLFWwindow* window, RenderState* state);
        void Update(float delta_time);
        void Shutdown();

    private:
        void DrawTitle();
        void DrawFPS(float delta_time);
        void DrawPolygonModeSettings();
        void DrawNormalMapsSettings();

    private:
        RenderState* state_ {nullptr};
    };
}

#endif