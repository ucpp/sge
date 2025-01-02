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
        io.DisplaySize = ImVec2((float)m_window->GetWidth(), (float)m_window->GetHeight());

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

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Quit"))
                {
                    // Implement exit functionality here
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render"))
            {
                if (ImGui::MenuItem("Settings"))
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
            if (ImGui::Begin("Settings", &m_isOpenSettingsWindow))
            {
                ImGui::Text("Rendering Settings");
                ImGui::Checkbox("Wireframe Mode", &m_settings->wireframeMode);

                ImGui::Text("LOD Level");
                ImGui::SliderFloat("##LODLevel", &m_settings->lodLevel, 0.0f, 10.0f, "%.1f");

                ImGui::End();
            }
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
}
