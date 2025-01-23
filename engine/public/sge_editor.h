#ifndef _SGE_EDITOR_H_
#define _SGE_EDITOR_H_

#include "pch.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "sge_descriptor_heap.h"
#include "sge_application_settings.h"
#include "sge_file_dialog.h"

namespace SGE
{
    class Editor
    {
    public:
        void Initialize(class RenderContext* context);
        void BuildFrame();
        void Render();
        void Shutdown();
        void SetActive(bool isActive);

    private:
        void SetupDockspace();
        void BuildDockingExample();

        void BuildMainMenuBar();
        void BuildSettingsWindow();
        void BuildResolutionWindow();

        void ApplyResolutionChange();

    private:
        class RenderContext* m_context = nullptr;

        bool m_isOpenSettingsWindow = false;
        bool m_isOpenResolutionWindow = false;

        std::unordered_map<AssetType, ImTextureID> m_icons;
        std::unordered_map<ObjectType, ImTextureID> m_objectIcons;

        std::unique_ptr<FileDialog> m_fileDialog;

        uint32 m_selectedObjectIndex = 0;

        bool m_isEnable = true;
    };
}

#endif // !_SGE_EDITOR_H_
