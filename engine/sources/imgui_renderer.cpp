#include "imgui_renderer.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "resource_manager.h"
#include "renderer.h"

namespace Engine
{
    void ImGuiRenderer::Init(GLFWwindow* window, RenderState* state)
    {
        state_ = state;
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
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

        DrawTitle();
        DrawFPS(delta_time);
        DrawPolygonModeSettings();
        DrawNormalMapsSettings();

        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiRenderer::DrawTitle()
    {
        ImGui::TextColored(ImColor(255, 255, 255, 255), "OpenGL render version 0.1");
    }

    void ImGuiRenderer::DrawFPS(float delta_time)
    {
        ImGui::TextColored(ImColor(255, 255, 255, 255), (std::to_string((int)(1.0f / delta_time)) + " fps").c_str());
    }

    void ImGuiRenderer::DrawPolygonModeSettings()
    {
        if(state_ != nullptr)
        {
            std::string button_name = state_->polygon_mode_enabled ? "Disable poligons" : "Enable polygins";
            if(ImGui::Button(button_name.c_str()))
            {
                state_->polygon_mode_enabled = !state_->polygon_mode_enabled;
                glPolygonMode(GL_FRONT_AND_BACK, state_->polygon_mode_enabled ? GL_FILL : GL_LINE);
            }
        }
    }

    void ImGuiRenderer::DrawNormalMapsSettings()
    {
        if(state_ != nullptr)
        {
            std::string button_name = state_->normal_maps_enabled ? "Disable normal maps" : "Enable normal maps";
            if(ImGui::Button(button_name.c_str()))
            {
                state_->normal_maps_enabled = !state_->normal_maps_enabled;
                state_->active_shader = state_->normal_maps_enabled ? 
                &Engine::ResourceManager::GetShader("lighting") :
                &Engine::ResourceManager::GetShader("basic");
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