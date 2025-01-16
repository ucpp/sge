#include "sge_editor.h"

#include "sge_application_settings.h"
#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_window.h"
#include "sge_render_context.h"
#include "sge_texture_manager.h"

namespace SGE
{
    static const std::string PathToSaveFile = "resources/configs/editor_layout.ini";

    void Editor::Initialize(RenderContext* context)
    {
        Verify(context, "Editor::Initialize failed: context object is null!");
        m_context = context;

        ID3D12Device* device = m_context->GetD12Device().Get();
        Verify(device, "Editor::Initialize failed: ID3D12Device is null!");

        HWND hwnd = m_context->GetWindowHandle();
        Verify(hwnd, "Editor::Initialize failed: HWND is null!");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = PathToSaveFile.c_str();

        ImGui_ImplWin32_EnableDpiAwareness();
        ImGui_ImplWin32_Init(hwnd);

        ID3D12DescriptorHeap* heap = m_context->GetCbvSrvUavHeap()->GetHeap().Get();
        Verify(heap, "Editor::Initialize failed: DescriptorHeap is null!");
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_context->GetCbvSrvUavHeap()->GetCPUHandle(EDITOR_START_HEAP_INDEX);
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_context->GetCbvSrvUavHeap()->GetGPUHandle(EDITOR_START_HEAP_INDEX);

        ImGui_ImplDX12_Init(device, BUFFER_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, heap, cpuHandle, gpuHandle);    

        uint32 meshIconIndex = TextureManager::GetTextureIndex("resources/editor/mesh_icon.png", m_context->GetDevice(), m_context->GetCbvSrvUavHeap());
        uint32 materialIconIndex = TextureManager::GetTextureIndex("resources/editor/material_icon.png", m_context->GetDevice(), m_context->GetCbvSrvUavHeap());
        uint32 lightIconIndex = TextureManager::GetTextureIndex("resources/editor/light_icon.png", m_context->GetDevice(), m_context->GetCbvSrvUavHeap());

        m_icons.emplace(AssetType::StaticMesh, (ImTextureID)m_context->GetCbvSrvUavHeap()->GetGPUHandle(meshIconIndex).ptr);
        m_icons.emplace(AssetType::Material, (ImTextureID)m_context->GetCbvSrvUavHeap()->GetGPUHandle(materialIconIndex).ptr);
        m_icons.emplace(AssetType::Light, (ImTextureID)m_context->GetCbvSrvUavHeap()->GetGPUHandle(lightIconIndex).ptr);
    }

    void Editor::BuildFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        SetupDockspace();
        BuildDockingExample();

        BuildMainMenuBar();
        BuildSettingsWindow();
        BuildResolutionWindow();
    }

    void Editor::BuildMainMenuBar()
    {
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
                    m_context->GetEditorSettings().isPressedQuit = true;
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
    }

    void Editor::BuildSettingsWindow()
    {
        if (m_isOpenSettingsWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(150, 300), ImGuiCond_Once);
            if (ImGui::Begin("Render settings", &m_isOpenSettingsWindow))
            {
                ImGui::Text("Render settings");
                RenderSettings& settings = m_context->GetRenderSettings();
                if (!settings.isDeferredRendering)
                {
                    ImGui::Checkbox("MSAA 4x", &settings.isMSAAEnabled);
                    ImGui::Checkbox("Fog", &settings.isFogEnabled);
                }
            }
            ImGui::End();
        }
    }

    void Editor::BuildResolutionWindow()
    {
        if (m_isOpenResolutionWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
            if (ImGui::Begin("Resolution Settings", &m_isOpenResolutionWindow))
            {
                ImGui::Text("Select Window Resolution:");
                static int32 selectedResolution = 0;
                const char* resolutions[] = { "640x480", "800x600", "1280x720", "1366x768", "1600x900", "1920x1080", "2560x1440" };
                if (ImGui::Combo("Resolution", &selectedResolution, resolutions, IM_ARRAYSIZE(resolutions)))
                {
                    ApplyResolutionChange(resolutions[selectedResolution]);
                }
            }
            ImGui::End();
        }
    }

    void Editor::ApplyResolutionChange(const std::string& resolution)
    {
        const size_t xPos = resolution.find('x');
        if (xPos != std::string::npos)
        {
            const int32 width = std::stoi(resolution.substr(0, xPos));
            const int32 height = std::stoi(resolution.substr(xPos + 1));

            m_context->SetWindowSize(width, height);
        }
    }

    void Editor::Render()
    {
        ImGui::Render();
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        Verify(commandList, "Editor::Render: 'commandList' is null or invalid. Rendering cannot proceed.");
        
        ComPtr<ID3D12DescriptorHeap> heap = m_context->GetCbvSrvUavHeap()->GetHeap();
        Verify(heap.Get(), "Editor::Render: DescriptorHeap is null or invalid. Rendering cannot proceed.");

        commandList->SetDescriptorHeaps(1, heap.GetAddressOf());
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    }

    void Editor::Shutdown()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
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

    void Editor::BuildDockingExample()
    {
        static uint32 selectedObjectIndex = 0;
        ImGui::Begin("Scene hierarchy");
        const SceneSettings& sceneSettings = m_context->GetSceneSettings();
        if (!sceneSettings.objects.empty())
        {
            float availableHeight = ImGui::GetContentRegionAvail().y;
            ImGui::BeginChild("ObjectsList", ImVec2(0, availableHeight), false);

            for (size_t i = 0; i < sceneSettings.objects.size(); ++i)
            {
                bool isSelected = selectedObjectIndex == i;
                if (ImGui::Selectable(sceneSettings.objects[i].name.c_str(), isSelected))
                {
                    selectedObjectIndex = static_cast<uint32>(i);
                }
            }

            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Begin("Content browser");
        const ProjectAssets& assetsSettings = m_context->GetAssetsSettings();
        ImGui::BeginChild("AssetScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        const float padding = 20.0f;
        const ImVec2 iconSize = ImVec2(64, 64);
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        float textHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;

        for (const auto& asset : assetsSettings.assets)
        {
            ImGui::SetCursorPos(ImVec2(xOffset, yOffset));

            ImGui::BeginGroup();

            std::string buttonId = "##" + asset.name;

            AssetType type = asset.type;
            ImTextureID iconTexture = m_icons[type];

            if (ImGui::ImageButton(buttonId.c_str(), iconTexture, iconSize))
            {
                std::cout << "Clicked on asset: " << asset.name << std::endl;
            }

            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize.x);
            ImGui::TextWrapped(asset.name.c_str());
            ImGui::PopTextWrapPos();

            ImGui::EndGroup();

            xOffset += iconSize.x + padding;
            if (xOffset + iconSize.x > availableWidth)
            {
                xOffset = 0.0f;
                yOffset += iconSize.y + textHeight + padding;
            }
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Properties");
        float width = ImGui::GetContentRegionAvail().x;

        static char objectName[128] = "";
        ImGui::Text("Name:");
        ImGui::SetNextItemWidth(width - 8.0f);
        ImGui::InputText("##name", objectName, IM_ARRAYSIZE(objectName), ImGuiInputTextFlags_EnterReturnsTrue);

        static float position[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::Text("Position:");
        ImGui::SetNextItemWidth(width - 8.0f);
        ImGui::InputFloat3("##position", position, "%.2f");

        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::Text("Rotation:");
        ImGui::SetNextItemWidth(width - 8.0f);
        ImGui::InputFloat3("##rotation", rotation, "%.2f");

        static float scale[3] = { 1.0f, 1.0f, 1.0f };
        ImGui::Text("Scale:");
        ImGui::SetNextItemWidth(width - 8.0f);
        ImGui::InputFloat3("##scale", scale, "%.2f");

        ImGui::End();
    }
}
