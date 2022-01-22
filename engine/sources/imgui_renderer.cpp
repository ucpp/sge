#include "imgui_renderer.h"

#include <string>
#include <sstream>

#include "resource_manager.h"
#include "editor.h"
#include "log.h"

namespace sge
{
    void ImGuiRenderer::initialize(std::weak_ptr<Window> window, RenderState *state, std::weak_ptr<Scene> scene)
    {
        this->state = state;
        this->scene = scene;
        this->window = window;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto s_ptr_window = window.lock();
        ImGui_ImplGlfw_InitForOpenGL(s_ptr_window->get(), true);

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
        ImGui::SetNextWindowSize(ImVec2(360, 400));
        ImGui::Begin("SGE v0.2.0", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        
        drawSettings(delta_time);

        ImGui::Separator();
        ImGui::BeginGroup();
        drawObjects();
        ImGui::EndGroup();

        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiRenderer::drawSettings(float delta_time)
    {
        if (!ImGui::CollapsingHeader("Settings"))
        {
            return;
        }
        drawFPS(delta_time);
        ImGui::SameLine();
        drawCountVertices();
        ImGui::Separator();
        drawScreenSettings();
        drawPolygonModeSettings();
        drawNormalMapsSettings();
    }
    
    void ImGuiRenderer::drawObjects()
    {
        if (!ImGui::CollapsingHeader("Objects"))
        {
            return;
        }
        auto sptr_scene = scene.lock();
        if (sptr_scene != nullptr)
        {
            ImGui::Text("Lights");
            ImGui::Checkbox("Directional light", &sptr_scene->getDirectionalLight()->data.enabled);
            ImGui::SameLine();
            auto& v3 = sptr_scene->getDirectionalLight()->data.position;
            float pos[3]{ v3.x, v3.y, v3.z };
            ImGui::SetNextItemWidth(160.0f);
            if (ImGui::DragFloat3("", pos, 0.2f, 0.0f, 0.0f, "%.2f"))
            {
                v3.x = pos[0];
                v3.y = pos[1];
                v3.z = pos[2];
            }
            ImGui::Separator();

            ImGui::Separator();
            ImGui::Text("Models");
            int index = 0;
            for (auto& obj : sptr_scene->objects)
            {
                index++;
                std::string label_text = std::to_string(index) + ". " + obj.name;
                ImGui::Checkbox(label_text.c_str(), &obj.enabled);
                ImGui::SameLine();
                if (ImGui::BeginPopup(label_text.c_str()))
                {
                    for (const std::string& shader_name : ResourceManager::getAllShaderNames())
                    {
                        if (ImGui::Button(shader_name.c_str(), ImVec2(90, 20)))
                        {
                            obj.material.shader = ResourceManager::getShader(shader_name);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::Button(obj.material.shader.name.c_str(), ImVec2(90, 20)))
                {
                    ImGui::OpenPopup(label_text.c_str());
                }
            }
        }
    }

    void ImGuiRenderer::drawFPS(float delta_time)
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (std::to_string((int)(1.0f / delta_time)) + " fps").c_str());
    }

    void ImGuiRenderer::drawCountVertices()
    {
        auto s_ptr_scene = scene.lock();
        std::string vertices = "vertices: " + std::to_string(s_ptr_scene.get()->getCountVertices());
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), vertices.c_str());
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

    void ImGuiRenderer::drawScreenSettings()
    {
        std::string label_text = "Fullscreen";
        auto s_ptr_window = window.lock();
        bool is_fullscreen = s_ptr_window->isFullscreen();
        if (ImGui::Checkbox(label_text.c_str(), &is_fullscreen))
        {
            s_ptr_window->setFullscreen(is_fullscreen);
        }
    }

    void ImGuiRenderer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}