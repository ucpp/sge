#ifndef RENDERER_H
#define RENDERER_H

namespace Engine
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
        void Init();
        void Render();

        void SetViewport(int width, int height);
    };
}

#endif