#ifndef _SGE_EDITOR_H_
#define _SGE_EDITOR_H_

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "sge_window.h"
#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_application_settings.h"

namespace SGE
{
    class Editor
    {
    public:
        void Initialize(Window* window, Device* device, ApplicationSettings* settings);
        void BuildImGuiFrame();
        void Render(ID3D12GraphicsCommandList* commandList);
        void Shutdown();

    private:
        Window* m_window = nullptr;
        Device* m_device = nullptr;
        ApplicationSettings* m_settings = nullptr;
        DescriptorHeap m_descriptorHeap;

        bool m_isOpenSettingsWindow = false;
    };
}

#endif // !_SGE_EDITOR_H_
