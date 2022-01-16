#pragma once

namespace sge
{
    class Shader;

    struct RenderState
    {
        bool polygon_mode_enabled = false;
        bool normal_maps_enabled = true;
    };

    class Renderer
    {
    public:
        void initialize();
        void render();

        void setViewport(int width, int height);
    };
}