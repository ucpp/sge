#include "imgui_renderer.h"

#include <string>
#include <sstream>

#include "resource_manager.h"
#include "editor.h"
#include "log.h"

namespace sge
{
    void ImGuiRenderer::initialize(GLFWwindow *window, RenderState *state, std::weak_ptr<Scene> scene)
    {
        this->state = state;
        this->scene = scene;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef __APPLE__
        ImGui_ImplOpenGL3_Init("#version 150");
#else
        ImGui_ImplOpenGL3_Init("#version 130");
#endif

        ImGui::StyleColorsDark();
    }

    void ImGuiRenderer::update(float delta_time)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), 1);
        ImGui::Begin("OpenGL render version 0.1", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

        drawFPS(delta_time);
        drawPolygonModeSettings();
        ImGui::SameLine();
        drawNormalMapsSettings();

        ImGui::Begin("Scene"); 
        auto sptr_scene = scene.lock();
        if (sptr_scene != nullptr)
        {
            int index = 0;
            for (auto& obj : sptr_scene->objects)
            {
                index++;
                std::string label_text = std::to_string(index) + ". " + obj.name;
                ImGui::Checkbox(label_text.c_str(), &obj.enabled);
                //ImGui::SameLine();
                //std::stringstream fmt;
                //fmt << "x:" << obj.position.x << ", y:" << obj.position.y << ", z:" << obj.position.z;
                //ImGui::Text(fmt.str().c_str());

            }
        }
        ImGui::End();

        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiRenderer::drawTitle()
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "OpenGL render version 0.1");
    }

    void ImGuiRenderer::drawFPS(float delta_time)
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (std::to_string((int)(1.0f / delta_time)) + " fps").c_str());
    }

    void ImGuiRenderer::drawPolygonModeSettings()
    {
        if (state != nullptr)
        {
            std::string label_text = "Wireframe";
            if (ImGui::Checkbox(label_text.c_str(), &state->polygon_mode_enabled))
            {
                glPolygonMode(GL_FRONT_AND_BACK, state->polygon_mode_enabled ? GL_LINE : GL_FILL);
            }
        }
    }

    void ImGuiRenderer::drawNormalMapsSettings()
    {
        if (state != nullptr)
        {
            std::string label_text = "Normal maps";
            if (ImGui::Checkbox(label_text.c_str(), &state->normal_maps_enabled))
            {
                auto sptr_scene = scene.lock();
                if(sptr_scene != nullptr)
                {
                    sptr_scene->enableNormalMaps(state->normal_maps_enabled);
                }
            }
        }
    }

    void ImGuiRenderer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}