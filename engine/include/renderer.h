#pragma once

namespace sge
{
    class Renderer
    {
    public:
        void initialize(bool vsync);
        void render();

        void setViewport(int width, int height);
    };
}