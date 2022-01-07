#ifndef RENDERER_H
#define RENDERER_H

namespace Engine
{
    class Shader;

    struct RenderState
    {
        Engine::Shader* active_shader;
        bool polygon_mode_enabled = true;
        bool normal_maps_enabled = true;
    };

    class Renderer
    {

    };
}

#endif