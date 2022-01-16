#include "imgui_renderer.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "resource_manager.h"
#include "renderer.h"

namespace sge
{
    void ImGuiRenderer::Init(GLFWwindow *window, RenderState *state, Scene *scene)
    {
        state_ = state;
        scene_ = scene;
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

    void ImGuiRenderer::Update(float delta_time)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), 1);
        ImGui::Begin("OpenGL render version 0.1", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

        DrawFPS(delta_time);
        DrawPolygonModeSettings();
        ImGui::SameLine();
        DrawNormalMapsSettings();

        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiRenderer::DrawTitle()
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "OpenGL render version 0.1");
    }

    void ImGuiRenderer::DrawFPS(float delta_time)
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (std::to_string((int)(1.0f / delta_time)) + " fps").c_str());
    }

    void ImGuiRenderer::DrawPolygonModeSettings()
    {
        if (state_ != nullptr)
        {
            std::string label_text = "Wireframe";
            if (ImGui::Checkbox(label_text.c_str(), &state_->polygon_mode_enabled))
            {
                glPolygonMode(GL_FRONT_AND_BACK, state_->polygon_mode_enabled ? GL_LINE : GL_FILL);
            }
        }
    }

    void ImGuiRenderer::DrawNormalMapsSettings()
    {
        if (state_ != nullptr)
        {
            std::string label_text = "Normal maps";
            if (ImGui::Checkbox(label_text.c_str(), &state_->normal_maps_enabled))
            {
                scene_->EnableNormalMaps(state_->normal_maps_enabled);
            }
        }
    }

    void ImGuiRenderer::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}