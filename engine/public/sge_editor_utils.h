#ifndef _SGE_EDITOR_UTILS_H_
#define _SGE_EDITOR_UTILS_H_

#include "pch.h"
#include "imgui.h"

namespace SGE
{
    bool InputText(const std::string& label, std::string& str, ImGuiInputTextFlags flags = 0);
    bool DragFloat(const std::string& label, float& value, float min = -FLT_MAX, float max = FLT_MAX, ImGuiInputTextFlags flags = 0);
    bool DragAngle3(const std::string& label, float3& value, ImGuiInputTextFlags flags = 0);
    bool DragFloat3(const std::string& label, float3& value, ImGuiInputTextFlags flags = 0);
    bool ColorEdit3(const std::string& label, float3& color, ImGuiInputTextFlags flags = 0);
}

#endif // !_SGE_EDITOR_UTILS_H_