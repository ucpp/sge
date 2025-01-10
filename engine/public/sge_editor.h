#ifndef _SGE_EDITOR_H_
#define _SGE_EDITOR_H_

#include "pch.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "sge_descriptor_heap.h"

namespace SGE
{
    class Editor
    {
    public:
        void Initialize(class RenderContext* context);
        void BuildFrame();
        void Render();
        void Shutdown();

        void SetupDockspace();
        void BuildDockingExample();

        void BuildMainMenuBar();
        void BuildSettingsWindow();
        void BuildResolutionWindow();

        void ApplyResolutionChange(const std::string& resolution);

    private:
        class RenderContext* m_context = nullptr;
        
        DescriptorHeap m_descriptorHeap;

        bool m_isOpenSettingsWindow = false;
        bool m_isOpenResolutionWindow = false;
    };
}

#endif // !_SGE_EDITOR_H_
