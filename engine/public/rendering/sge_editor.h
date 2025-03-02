#ifndef _SGE_EDITOR_H_
#define _SGE_EDITOR_H_

#include "pch.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_file_dialog.h"
#include "data/sge_data_structures.h"
#include "data/sge_model_asset.h"

namespace SGE
{
    class Editor
    {
    public:
        void Initialize(class RenderContext* context, class Scene* scene);
        void BuildFrame();
        void Render();
        void Shutdown();
        void SetActive(bool isActive);

    private:
        void InitializeImGuiDX12();
        void RegisterAssetIcons();
        void RegisterObjectIcons();

        void BeginNewFrame();
        void SetupDockspace();

        void ConstructEditors();

        void ConstructMenuBar();
        void ConstructFileMenu();
        void ConstructEditMenu();
        void ConstructWindowMenu();
        void ConstructHelpMenu();
        void ConstructSceneObjectsList();
        void ConstructPropertiesEditor();
        void ConstructContentBrowser();
        void ConstructWindowSettings();

        void ConstructAnimationEditor();
    
        uint32 GetTextureIndex(const std::string& name) const;
        ImTextureID GetTexturePtr(const std::string& name) const;
        ImTextureID GetTexturePtr(uint32 index) const;

        void DisplayBoneHierarchy(Bone& bone, int level, Skeleton& skeleton);
        void ApplyWeightToChildren(Bone& bone, Skeleton& skeleton, float weight);

    private:
        void OnSave();
        void OnSaveAs();
        void OnExit();
        void OnResolutionChange();

    private:
        class RenderContext* m_context = nullptr;
        class Scene* m_activeScene = nullptr;
        class AnimatedModelInstance* m_activeAnimatedModel = nullptr;

        bool m_isEnableWindowSettings = false;

        std::unordered_map<AssetType, ImTextureID>  m_assetIcons;
        std::unordered_map<ObjectType, ImTextureID> m_objectIcons;
        
        ImTextureID m_invisibleObjectTexure;
        ImTextureID m_visibleObjectTexure;
        ImTextureID m_playButtonTexure;
        ImTextureID m_pauseButtonTexure;
        ImTextureID m_stopButtonTexure;
        ImTextureID m_boneTexure;

        std::unique_ptr<FileDialog> m_fileDialog;

        ObjectDataBase* m_selectedObject = nullptr; 
        int32 m_selectedObjectIndex = -1;
        int32 m_selectedAnimationIndex = -1;
        int32 m_currentLayer = 0;
        bool m_applyWeightToChildren = false;
        bool m_initedWeights = false;
        std::vector<std::string> m_animationNames;

        bool m_isEnable = true;
    };

    std::string ConvertWcharToUtf8(const wchar_t* wcharStr);
}

#endif // !_SGE_EDITOR_H_
