#include "sge_editor.h"

#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_window.h"
#include "sge_render_context.h"
#include "sge_texture_manager.h"
#include "sge_config.h"
#include "sge_data_structures.h"
#include "sge_scoped_event.h"

#include <commdlg.h>

namespace SGE
{
    static const std::string PathToSaveFile = "resources/configs/editor_layout.ini";
    static const std::string PathToIcons = "resources/editor/";

    std::string ConvertWcharToUtf8(const wchar_t* wcharStr)
    {
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, nullptr, 0, nullptr, nullptr);
        std::vector<char> utf8Str(sizeNeeded);
        WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, utf8Str.data(), sizeNeeded, nullptr, nullptr);
        
        return std::string(utf8Str.begin(), utf8Str.end());
    }

    void Editor::Initialize(RenderContext* context)
    {
        Verify(context, "Editor::Initialize failed: context object is null!");
        m_context = context;

        ID3D12Device* device = m_context->GetD12Device().Get();
        Verify(device, "Editor::Initialize failed: ID3D12Device is null!");

        HWND hwnd = m_context->GetWindowHandle();
        Verify(hwnd, "Editor::Initialize failed: HWND is null!");

        m_fileDialog = std::make_unique<FileDialog>(hwnd);

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

        m_icons.emplace(AssetType::Model, GetTexturePtr("mesh_icon.png"));
        m_icons.emplace(AssetType::Material, GetTexturePtr("material_icon.png"));
        m_icons.emplace(AssetType::Light, GetTexturePtr("light_icon.png"));

        m_objectIcons.emplace(ObjectType::Camera, GetTexturePtr("camera_16.png"));
        m_objectIcons.emplace(ObjectType::DirectionalLight, GetTexturePtr("directional_light_16.png"));
        m_objectIcons.emplace(ObjectType::PointLight, GetTexturePtr("point_light_16.png"));
        m_objectIcons.emplace(ObjectType::Model, GetTexturePtr("mesh_16.png"));

        m_visibleObjectTexure   = GetTexturePtr("visible_16.png");
        m_invisibleObjectTexure = GetTexturePtr("invisible_16.png");
    }

    uint32 Editor::GetTextureIndex(const std::string& name) const
    {
        const DescriptorHeap* heap = m_context->GetCbvSrvUavHeap();
        const Device* device = m_context->GetDevice();

        return TextureManager::GetTextureIndex(PathToIcons + name, TextureType::Default, device, heap);
    }

    ImTextureID Editor::GetTexturePtr(const std::string& name) const
    {
        const uint32 index = GetTextureIndex(name);
        return GetTexturePtr(index);
    }

    ImTextureID Editor::GetTexturePtr(uint32 index) const
    {
        return static_cast<ImTextureID>(m_context->GetCbvSrvUavHeap()->GetGPUHandle(index).ptr);
    }

    void Editor::BuildFrame()
    {
        if(!m_isEnable)
        {
            return;
        }

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        SetupDockspace();
        BuildDockingExample();

        BuildMainMenuBar();
        BuildSettingsWindow();
        BuildResolutionWindow();
    }

    void Editor::Render()
    {
        if(!m_isEnable)
        {
            return;
        }
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        Verify(commandList, "Editor::Render: 'commandList' is null or invalid. Rendering cannot proceed.");

        SCOPED_EVENT("Draw Editor UI");
        ImGui::Render();
        
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

    void Editor::SetActive(bool isActive)
    {
        m_isEnable = isActive;
    }

    void Editor::BuildMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Save"))
                {
                    Config::Save<ApplicationData>(DEFAULT_SETTINGS_PATH, m_context->GetApplicationData());
                }
                if(ImGui::MenuItem("Save as"))
                {
                    if (m_fileDialog->SaveAs(L"json", L"JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0"))
                    {
                        std::string savePath = m_fileDialog->GetSavePath();
                        Config::Save<ApplicationData>(savePath, m_context->GetApplicationData());
                        LOG_INFO("Settings saved to: {}", savePath);
                    }
                    else
                    {
                        DWORD error = CommDlgExtendedError();
                        if (error != 0)
                        {
                            LOG_ERROR("Error in Save As dialog: {}", error);
                        }
                        else
                        {
                            LOG_INFO("Save As canceled.");
                        }
                    }
                }

                if (ImGui::MenuItem("Settings"))
                {
                    m_isOpenResolutionWindow = true;
                }
                if (ImGui::MenuItem("Quit"))
                {
                    m_context->GetWindowData().isPressedQuit = true;
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
                RenderData& renderData = m_context->GetRenderData();
                if (!renderData.isDeferredRendering)
                {
                    ImGui::Checkbox("MSAA 4x", &renderData.isMSAAEnabled);
                    ImGui::Checkbox("Fog", &renderData.isFogEnabled);
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
                WindowData& windowData = m_context->GetWindowData();
                ImGui::Text("Select Window Resolution:");
                const int32 size = static_cast<int32>(windowData.resolutions.size());
                if (ImGui::Combo("Resolution", &windowData.selectedResolution, windowData.GetResolutions(), size))
                {
                    ApplyResolutionChange();
                }
            }
            ImGui::End();
        }
    }

    void Editor::ApplyResolutionChange()
    {
        WindowData& windowData = m_context->GetWindowData();
        const int32 width = windowData.GetWidth();
        const int32 height = windowData.GetHeight();

        m_context->SetWindowSize(width, height);
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
        ImGui::Begin("Scene hierarchy");
        const ImVec2 smallIconSize = ImVec2(16, 16);
        const SceneData& sceneSettings = m_context->GetSceneData();

        if (!sceneSettings.objects.empty())
        {
            float availableHeight = ImGui::GetContentRegionAvail().y;
            ImGui::BeginChild("ObjectsList", ImVec2(0, availableHeight), false);

            uint32 index = 0;
            for (const auto& [type, objectList] : sceneSettings.objects)
            {
                for (const auto& obj : objectList)
                {
                    bool isSelected = (m_selectedObjectIndex == index);

                    ImGui::Image(m_objectIcons[type], smallIconSize);
                    ImGui::SameLine();

                    if (ImGui::Selectable(obj->name.c_str(), isSelected))
                    {
                        m_selectedObjectIndex = index;
                        m_selectedObject = obj.get();
                    }
                    
                    ImGui::SameLine();
                    float offset = ImGui::GetWindowWidth() - smallIconSize.x;
                    ImGui::SetCursorPosX(offset);

                    ImTextureID visibilityIcon = obj->enabled ? m_visibleObjectTexure : m_invisibleObjectTexure;
                    ImGui::Image(visibilityIcon, smallIconSize);
            
                    ++index;
                }
            }

            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Begin("Content browser");
        const AssetsData& assetsSettings = m_context->GetAssetsData();
        ImGui::BeginChild("AssetScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        const float padding = 20.0f;
        const ImVec2 iconSize = ImVec2(64, 64);
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        float textHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;

        for (const auto& [type, assetMap] : assetsSettings.assets)
        {
            for (const auto& [id, asset] : assetMap)
            {
                ImGui::SetCursorPos(ImVec2(xOffset, yOffset));

                ImGui::BeginGroup();

                std::string buttonId = "##" + asset->name;

                ImTextureID iconTexture = m_icons[type];

                if (ImGui::ImageButton(buttonId.c_str(), iconTexture, iconSize))
                {
                    std::cout << "Clicked on asset: " << asset->name << std::endl;
                }

                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize.x);
                ImGui::TextWrapped(asset->name.c_str());
                ImGui::PopTextWrapPos();

                ImGui::EndGroup();

                xOffset += iconSize.x + padding;
                if (xOffset + iconSize.x > availableWidth)
                {
                    xOffset = 0.0f;
                    yOffset += iconSize.y + textHeight + padding;
                }
            }
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Properties");
        if (m_selectedObject)
        {
            m_selectedObject->DrawEditor();
        }
        else
        {
            ImGui::Text("No object selected.");
        }
        ImGui::End();
    }
}
