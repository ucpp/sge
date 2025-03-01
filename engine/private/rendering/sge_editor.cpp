#include "rendering/sge_editor.h"

#include "core/sge_descriptor_heap.h"
#include "core/sge_device.h"
#include "core/sge_helpers.h"
#include "core/sge_window.h"
#include "rendering/sge_render_context.h"
#include "data/sge_texture_manager.h"
#include "core/sge_config.h"
#include "data/sge_data_structures.h"
#include "core/sge_scoped_event.h"
#include "data/sge_scene.h"
#include "data/sge_animated_model_instance.h"

#include <commdlg.h>

namespace SGE
{
    static const std::string PathToSaveFile = "resources/configs/editor_layout.ini";
    static const std::string PathToIcons = "resources/editor/";

    void Editor::Initialize(RenderContext* context, Scene* scene)
    {
        Verify(context, "Editor::Initialize failed: context object is null!");
        m_context = context;
        m_activeScene = scene;

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

        InitializeImGuiDX12();
        RegisterAssetIcons();
        RegisterObjectIcons();
    }

    void Editor::InitializeImGuiDX12()
    {
        ID3D12Device* device = m_context->GetD12Device().Get();
        Verify(device, "Editor::Initialize failed: ID3D12Device is null!");

        ID3D12DescriptorHeap* heap = m_context->GetCbvSrvUavHeap()->GetHeap().Get();
        Verify(heap, "Editor::Initialize failed: DescriptorHeap is null!");

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_context->GetCbvSrvUavHeap()->GetCPUHandle(EDITOR_START_HEAP_INDEX);
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_context->GetCbvSrvUavHeap()->GetGPUHandle(EDITOR_START_HEAP_INDEX);

        ImGui_ImplDX12_Init(device, BUFFER_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, heap, cpuHandle, gpuHandle);
    }

    void Editor::RegisterAssetIcons()
    {
        m_assetIcons.emplace(AssetType::Model, GetTexturePtr("mesh_icon.png"));
        m_assetIcons.emplace(AssetType::AnimatedModel, GetTexturePtr("anim_icon.png"));
        m_assetIcons.emplace(AssetType::Material, GetTexturePtr("material_icon.png"));
        m_assetIcons.emplace(AssetType::Light, GetTexturePtr("light_icon.png"));
        m_assetIcons.emplace(AssetType::Cubemap, GetTexturePtr("cubemap_icon.png"));
    }

    void Editor::RegisterObjectIcons()
    {
        m_objectIcons.emplace(ObjectType::Camera, GetTexturePtr("camera_16.png"));
        m_objectIcons.emplace(ObjectType::DirectionalLight, GetTexturePtr("directional_light_16.png"));
        m_objectIcons.emplace(ObjectType::PointLight, GetTexturePtr("point_light_16.png"));
        m_objectIcons.emplace(ObjectType::Model, GetTexturePtr("mesh_16.png"));
        m_objectIcons.emplace(ObjectType::AnimatedModel, GetTexturePtr("anim_16.png"));
        m_objectIcons.emplace(ObjectType::Skybox, GetTexturePtr("skybox_16.png"));

        m_visibleObjectTexure   = GetTexturePtr("visible_16.png");
        m_invisibleObjectTexure = GetTexturePtr("invisible_16.png");
        m_playButtonTexure      = GetTexturePtr("play_16.png");
        m_pauseButtonTexure     = GetTexturePtr("pause_16.png");
        m_stopButtonTexure      = GetTexturePtr("stop_16.png");
        m_boneTexure            = GetTexturePtr("bone.png");
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

        BeginNewFrame();
        SetupDockspace();
        
        ConstructEditors();
        ConstructMenuBar();
    }

    void Editor::BeginNewFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void Editor::Render()
    {
        if(!m_isEnable)
        {
            return;
        }

        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        Verify(commandList, "Editor::Render: 'commandList' is null or invalid. Rendering cannot proceed.");

        SCOPED_EVENT_GPU(commandList, "Draw Editor UI");
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

    void Editor::ConstructMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            ConstructFileMenu();
            ConstructEditMenu();
            ConstructWindowMenu();
            ConstructHelpMenu();

            ImGui::EndMainMenuBar();
        }
    }

    void Editor::ConstructFileMenu()
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Save"))
            {
                OnSave();
            }
            
            if(ImGui::MenuItem("Save as"))
            {
                OnSaveAs();
            }
            
            if (ImGui::MenuItem("Exit"))
            {
                OnExit();
            }

            ImGui::EndMenu();
        }
    }

    void Editor::ConstructEditMenu()
    {
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo")) {}
            if (ImGui::MenuItem("Redo")) {}
            ImGui::EndMenu();
        }
    }

    void Editor::ConstructWindowMenu()
    {
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Window settings"))
            {
                m_isEnableWindowSettings = true;
            }
            
            ImGui::EndMenu();
        }
    }

    void Editor::ConstructHelpMenu()
    {
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About SGE")) {}
            ImGui::EndMenu();
        }
    }

    void Editor::OnSave()
    {
        Config::Save<ApplicationData>(DEFAULT_SETTINGS_PATH, m_context->GetApplicationData());
    }

    void Editor::OnSaveAs()
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

    void Editor::OnExit()
    {
        m_context->GetWindowData().isPressedQuit = true;
    }

    void Editor::ConstructWindowSettings()
    {
        if (!m_isEnableWindowSettings)
        {
            return;
        }

        constexpr ImVec2 WINDOW_SIZE(280.0f, 80.0f);
        ImGui::SetNextWindowSize(WINDOW_SIZE, ImGuiCond_Once);

        if (ImGui::Begin("Window settings", &m_isEnableWindowSettings))
        {
            WindowData& windowData = m_context->GetWindowData();

            constexpr float LABEL_WIDTH = 90.0f;
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Resolution:");
            ImGui::SameLine(LABEL_WIDTH);

            const int32 resolutionCount = static_cast<int32>(windowData.resolutions.size());
            if (ImGui::Combo("##Resolution", &windowData.selectedResolution, windowData.GetResolutions(), resolutionCount))
            {
                OnResolutionChange();
            }

            ImGui::End();
        }
    }

    void Editor::DisplayBoneHierarchy(const Bone& bone, int level, Skeleton& skeleton)
    {
        std::string label(level * 1, ' ');
        label += bone.name + " (" + std::to_string(bone.layer) + ")";

        if (bone.index >= m_selectedBones.size()) 
        {
            m_selectedBones.resize(bone.index + 1, false);
        }

        bool value = m_selectedBones[bone.index];
        if (ImGui::Checkbox(label.c_str(), &value)) 
        {
            m_selectedBones[bone.index] = value;
        }

        for (int32 childIndex : bone.children)
        {
            const Bone& childBone = skeleton.GetBone(childIndex);
            DisplayBoneHierarchy(childBone, level + 1, skeleton);
        }
    }

    void Editor::ConstructAnimationEditor()
    {
        if (m_activeAnimatedModel)
        {
            if (m_selectedObject)
            {
                auto* animatedModelData = dynamic_cast<AnimatedModelData*>(m_selectedObject);
                if (animatedModelData)
                {
                    Skeleton& skeleton = m_activeAnimatedModel->GetSkeleton();
                    for (int i = 0; i < skeleton.GetBoneCount(); ++i)
                    {
                        Bone& bone = skeleton.GetBone(i);
                        const std::string& boneName = bone.name;
                        if (animatedModelData->boneLayers.find(boneName) != animatedModelData->boneLayers.end())
                        {
                            bone.layer = animatedModelData->boneLayers[boneName];
                        }
                    }
                }
            }

            ImGui::Separator();
            ImGui::Text("Animation Layers:");
            ImGui::Separator();

            const std::vector<Animation>& animations = m_activeAnimatedModel->GetAnimations();

            if (m_animationNames.empty() && !animations.empty())
            {
                m_animationNames.clear();
                for (const auto& animation : animations)
                {
                    m_animationNames.push_back(animation.name);
                }
                m_selectedAnimationIndex = 0;
            }

            float availableWidth = ImGui::GetContentRegionAvail().x;

            static const char* layers[] = { "Layer 0", "Layer 1", "Layer 2" };
            static int currentLayer = 0;
            static float blendWeight = 0.5f;

            ImGui::SetNextItemWidth(availableWidth * 0.5f);
            ImGui::Combo("##LayerSelector", &currentLayer, layers, IM_ARRAYSIZE(layers));
            ImGui::SameLine();

            if (ImGui::Button("Apply Layer", ImVec2(availableWidth * 0.5f - 8.0f, 0)))
            {
                if (m_selectedObject)
                {
                    auto* animatedModelData = dynamic_cast<AnimatedModelData*>(m_selectedObject);
                    if (animatedModelData)
                    {
                        Skeleton& skeleton = m_activeAnimatedModel->GetSkeleton();
                        for (int i = 0; i < skeleton.GetBoneCount(); ++i)
                        {
                            if (m_selectedBones[i])
                            {
                                Bone& bone = skeleton.GetBone(i);
                                bone.layer = currentLayer;
                                animatedModelData->boneLayers[bone.name] = bone.layer;
                            }
                        }
                    }
                }
            }

            ImGui::Text("Add Animation to Layer:");
            ImGui::SetNextItemWidth(availableWidth);
            if (ImGui::BeginCombo("##AnimationSelector", m_animationNames[m_selectedAnimationIndex].c_str()))
            {
                for (int i = 0; i < m_animationNames.size(); ++i)
                {
                    bool isSelected = (m_selectedAnimationIndex == i);
                    if (ImGui::Selectable(m_animationNames[i].c_str(), isSelected))
                    {
                        m_selectedAnimationIndex = i;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SetNextItemWidth(availableWidth);
            ImGui::SliderFloat("##BlendWeight", &blendWeight, 0.0f, 1.0f, "Weight: %.2f");
            if (ImGui::Button("Add Animation to Layer"))
            {
                m_activeAnimatedModel->SelectAnimationForLayer(m_animationNames[m_selectedAnimationIndex], currentLayer);
                m_activeAnimatedModel->SetAnimationWeightForLayer(currentLayer, blendWeight);
                m_activeAnimatedModel->PlayAnimationForLayer(currentLayer);
            }

            float ticksPerSecond = m_activeAnimatedModel->GetTicksPerSecondForLayer(currentLayer);
            float currentTime = m_activeAnimatedModel->GetCurrentAnimationTimeForLayer(currentLayer);
            float duration = m_activeAnimatedModel->GetCurrentAnimationDurationForLayer(currentLayer);
            ImGui::Text("Time (sec): %.2f / %.2f", currentTime / ticksPerSecond, duration / ticksPerSecond);

            ImVec2 buttonSize(16, 16);
            if (ImGui::ImageButton("##Play", (ImTextureID)m_playButtonTexure, buttonSize))
            {
                m_activeAnimatedModel->PlayAnimationForLayer(currentLayer);
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("##Pause", (ImTextureID)m_pauseButtonTexure, buttonSize))
            {
                m_activeAnimatedModel->StopAnimationForLayer(currentLayer);
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("##Stop", (ImTextureID)m_stopButtonTexure, buttonSize))
            {
                m_activeAnimatedModel->StopAnimationForLayer(currentLayer);
                m_activeAnimatedModel->ResetAnimationTimeForLayer(currentLayer);
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
            ImGui::SetNextItemWidth(availableWidth - 96);
            ImGui::SameLine();
            if (ImGui::SliderFloat("##Progress", &currentTime, 0.0f, duration, ""))
            {
                m_activeAnimatedModel->SetCurrentAnimationTimeForLayer(currentLayer, currentTime);
                m_activeAnimatedModel->FixedUpdate(0.0f, true);
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            ImGui::Separator();

            if (m_selectedBones.empty())
            {
                m_selectedBones.resize(m_activeAnimatedModel->GetSkeleton().GetBoneCount(), false);
            }

            if (ImGui::TreeNode("Bone Hierarchy"))
            {
                ImGui::Unindent();
                for (const auto& bone : m_activeAnimatedModel->GetSkeleton().GetBones())
                {
                    if (bone.parentIndex == -1)
                    {
                        DisplayBoneHierarchy(bone, 0, m_activeAnimatedModel->GetSkeleton());
                    }
                }
                ImGui::Indent();
                ImGui::TreePop();
            }
        }
        else
        {
            if (!m_animationNames.empty())
            {
                m_animationNames.clear();
            }
            if (!m_selectedBones.empty())
            {
                m_selectedBones.clear();
            }
        }
    }

    void Editor::OnResolutionChange()
    {
        WindowData& windowData = m_context->GetWindowData();
        const int32 width = windowData.GetWidth();
        const int32 height = windowData.GetHeight();

        m_context->SetWindowSize(width, height);
    }

    void Editor::SetupDockspace()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        constexpr float TRANSPARENT_BG = 0.0f;
        constexpr ImVec2 NO_PADDING(0.0f, 0.0f);
        constexpr ImVec2 FULLSIZE_DOCKSPACE(0.0f, 0.0f);

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(TRANSPARENT_BG);

        constexpr ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, NO_PADDING);
        
        if (ImGui::Begin("Main DockSpace", nullptr, WINDOW_FLAGS))
        {
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, FULLSIZE_DOCKSPACE, ImGuiDockNodeFlags_PassthruCentralNode);
        }
        
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Editor::ConstructEditors()
    {
        ConstructSceneObjectsList();
        ConstructContentBrowser();
        ConstructPropertiesEditor();
        ConstructWindowSettings();
    }

    void Editor::ConstructSceneObjectsList()
    {
        ImGui::Begin("Scene hierarchy");

        RenderData& renderData = m_context->GetRenderData();
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - 16.0f);

        const char* techniqueNames[] = { "Forward", "Deferred" };
        int32 currentTechnique = static_cast<int32>(renderData.technique);
        if (ImGui::Combo("##Technique", &currentTechnique, techniqueNames, IM_ARRAYSIZE(techniqueNames)))
        {
            renderData.technique = static_cast<RenderTechnique>(currentTechnique);
        }

        const std::vector<const char*>& passNames = (renderData.technique == RenderTechnique::Forward)
            ? renderData.GetForwardOutputsData()
            : renderData.GetDeferredOutputsData();

        auto it = std::find(passNames.begin(), passNames.end(), renderData.finalRender);
        int32 currentPass = (it != passNames.end()) ? static_cast<int32>(std::distance(passNames.begin(), it))
                                                    : static_cast<int32>(passNames.size() - 1);

        renderData.finalRender = passNames.empty() ? "tonemapping_target" : std::string(passNames[currentPass]);


        if (!passNames.empty() && ImGui::Combo("##RenderPass", &currentPass, passNames.data(), static_cast<int32>(passNames.size())))
        {
            renderData.finalRender = passNames[currentPass];
        }

        ImGui::PopItemWidth();

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f)); 

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
                    
                    ImGui::BeginGroup();
                    ImGui::Image(m_objectIcons[type], smallIconSize);
                    ImGui::SameLine();
                    const auto objName = obj->name.empty() ? "empty" : obj->name.c_str();
                    if (ImGui::Selectable(objName, isSelected))
                    {
                        m_selectedObjectIndex = index;
                        m_selectedObject = obj.get();
                        
                        if(m_selectedObject->type == ObjectType::AnimatedModel && m_activeScene)
                        {
                            AnimatedModelData* animatedModel = dynamic_cast<AnimatedModelData*>(m_selectedObject);
                            if (animatedModel)
                            {
                                m_activeAnimatedModel = m_activeScene->GetAnimModel(animatedModel);
                            }
                        }
                        else
                        {
                            m_activeAnimatedModel = nullptr;
                        }
                    }
                    
                    ImGui::SameLine();
                    float offset = ImGui::GetWindowWidth() - smallIconSize.x;
                    ImGui::SetCursorPosX(offset);

                    ImTextureID visibilityIcon = obj->enabled ? m_visibleObjectTexure : m_invisibleObjectTexure;
                    ImGui::Image(visibilityIcon, smallIconSize);
                    ImGui::EndGroup();

                    ++index;
                }
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }

    void Editor::ConstructPropertiesEditor()
    {
        ImGui::Begin("Properties");
        if (m_selectedObject)
        {
            m_selectedObject->DrawEditor();
            ConstructAnimationEditor();
        }
        else
        {
            ImGui::Text("No object selected.");
        }
        ImGui::End();
    }

    void Editor::ConstructContentBrowser()
    {
        constexpr float PADDING = 20.0f;
        constexpr ImVec2 ICON_SIZE(64.0f, 64.0f);
        
        ImGui::Begin("Content Browser");
        const AssetsData& assetsSettings = m_context->GetAssetsData();
        ImGui::BeginChild("AssetScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float textHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;

        float xOffset = 0.0f;
        float yOffset = 0.0f;

        for (const auto& [type, assetMap] : assetsSettings.assets)
        {
            for (const auto& [id, asset] : assetMap)
            {
                ImGui::SetCursorPos(ImVec2(xOffset, yOffset));
                ImGui::BeginGroup();

                std::string buttonId = "##" + asset->name;
                ImTextureID iconTexture = m_assetIcons[type];

                if (ImGui::ImageButton(buttonId.c_str(), iconTexture, ICON_SIZE))
                {
                    std::cout << "Clicked on asset: " << asset->name << std::endl;
                }

                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ICON_SIZE.x);
                ImGui::TextWrapped(asset->name.c_str());
                ImGui::PopTextWrapPos();

                ImGui::EndGroup();

                xOffset += ICON_SIZE.x + PADDING;
                if (xOffset + ICON_SIZE.x > availableWidth)
                {
                    xOffset = 0.0f;
                    yOffset += ICON_SIZE.y + textHeight + PADDING;
                }
            }
        }

        ImGui::EndChild();
        ImGui::End();
    }

    std::string ConvertWcharToUtf8(const wchar_t* wcharStr)
    {
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, nullptr, 0, nullptr, nullptr);
        std::vector<char> utf8Str(sizeNeeded);
        WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, utf8Str.data(), sizeNeeded, nullptr, nullptr);
        
        return std::string(utf8Str.begin(), utf8Str.end());
    }
}
