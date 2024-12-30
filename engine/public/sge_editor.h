#ifndef _SGE_EDITOR_H_
#define _SGE_EDITOR_H_

#include "pch.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "sge_descriptor_heap.h"

namespace SGE
{
    class Editor
    {
    public:
        void Initialize(class Window* window, class Device* device, struct ApplicationSettings* settings);
        void BuildImGuiFrame();
        void Render(ID3D12GraphicsCommandList* commandList);
        void Shutdown();

    private:
        class Window* m_window = nullptr;
        class Device* m_device = nullptr;
        struct ApplicationSettings* m_settings = nullptr;
        DescriptorHeap m_descriptorHeap;

        bool m_isOpenSettingsWindow = false;
    };
}

#endif // !_SGE_EDITOR_H_
