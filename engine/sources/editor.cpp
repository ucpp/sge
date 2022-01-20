#include "editor.h"

namespace sge
{
    void Editor::initialize(std::weak_ptr<Window> window, std::weak_ptr<Scene> scene, bool draw_gui)
    {
        this->draw_gui = draw_gui;
        imgui_renderer.initialize(window, &state, scene);
    }

    void Editor::update(float delta_time)
    {
        if(draw_gui)
        {
            imgui_renderer.update(delta_time);
        }
    }

    void Editor::shutdown()
    {
        imgui_renderer.shutdown();
    }
}