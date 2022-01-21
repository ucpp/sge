#pragma once

#include <glm/glm.hpp>

namespace sge
{
    class Renderer
    {
    public:
        Renderer();
        void initialize(bool vsync);
        void render();

        void setViewport(int width, int height);
        void setClearColor(glm::vec3 color);

    private:
        void clearScreen();

    private:
        glm::vec3 clear_color;
    };
}