#include "sge_editor.h"

#include "sge_application_settings.h"
#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_window.h"

namespace SGE
{
    void Editor::Initialize(Window* window, Device* device, ApplicationSettings* settings)
    {
        m_window = window;
        m_device = device;
        m_settings = settings;

        m_descriptorHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = "resources/configs/editor_layout.ini";

        ImGui_ImplWin32_EnableDpiAwareness();
        ImGui_ImplWin32_Init(m_window->GetHandle());
        ImGui_ImplDX12_Init(
            m_device->GetDevice().Get(),
            BufferCount,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            m_descriptorHeap.GetHeap().Get(),
            m_descriptorHeap.GetCPUHandle(0),
            m_descriptorHeap.GetGPUHandle(0)
        );     
    }

    void Editor::BuildImGuiFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        SetupDockspace();
        ShowDockingExample();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Settings"))
                {
                    m_isOpenResolutionWindow = true;
                }
                if (ImGui::MenuItem("Quit"))
                {
                    m_settings->isPressedQuit = true;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render"))
            {
                if (ImGui::MenuItem("Render settings"))
                {
                    m_isOpenSettingsWindow = true;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (m_isOpenSettingsWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(150, 300), ImGuiCond_Once);
            if (ImGui::Begin("Render settings", &m_isOpenSettingsWindow))
            {
                ImGui::Text("Render settings");
                ImGui::Checkbox("MSAA 4x", &m_settings->isMSAAEnabled);
                ImGui::Checkbox("Fog", &m_settings->isFogEnabled);
            }
            ImGui::End();
        }

        if (m_isOpenResolutionWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
            if (ImGui::Begin("Resolution Settings", &m_isOpenResolutionWindow))
            {
                ImGui::Text("Select Window Resolution:");
                static int32 selectedResolution = 0;
                const char* resolutions[] = {
                    "640x480",
                    "800x600",
                    "1024x768",
                    "1280x720",
                    "1366x768",
                    "1600x900",
                    "1920x1080",
                    "2560x1440"
                };
                if (ImGui::Combo("Resolution", &selectedResolution, resolutions, IM_ARRAYSIZE(resolutions)))
                {
                    // Apply the resolution change
                    std::string resString = resolutions[selectedResolution];
                    size_t xPos = resString.find('x');
                    if (xPos != std::string::npos)
                    {
                        int width = std::stoi(resString.substr(0, xPos));
                        int height = std::stoi(resString.substr(xPos + 1));

                        m_window->SetWindowSize(width, height);
                    }

                }
            }
            ImGui::End();
        }
    }

    void Editor::Render(ID3D12GraphicsCommandList* commandList)
    {
        ImGui::Render();
        commandList->SetDescriptorHeaps(1, m_descriptorHeap.GetHeap().GetAddressOf());
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    }

    void Editor::Shutdown()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        m_descriptorHeap.Shutdown();
    }

    void Editor::SetupDockspace()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Main DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();

        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }

    void Editor::ShowDockingExample()
    {
        ImGui::Begin("Scene hierarchy");
        ImGui::End();

        ImGui::Begin("Content browser");
        ImGui::End();
    }
}
