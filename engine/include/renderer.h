#ifndef RENDERER_H
#define RENDERER_H

namespace Engine
{
    class Shader;

    struct RenderState
    {
        Engine::Shader* active_shader;
        bool polygon_mode_enabled = false;
        bool normal_maps_enabled = true;
    };

    class Renderer
    {

    };
}

#endif