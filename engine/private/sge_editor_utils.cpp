#include "sge_editor_utils.h"

namespace SGE
{
    namespace
    {
        constexpr size_t INPUT_BUFFER_SIZE = 1024;
        constexpr float DEFAULT_DRAG_SPEED = 0.01f;
        constexpr float DEFAULT_DRAG_MIN = -FLT_MAX;
        constexpr float DEFAULT_DRAG_MAX = FLT_MAX;
        constexpr float ANGLE_MODULO = 360.0f;
        constexpr float LABEL_OFFSET = 16.0f;
        constexpr float ITEM_WIDTH_RATIO = 0.5f;
    }

    bool InputText(const std::string& label, std::string& str, ImGuiInputTextFlags flags)
    {
        float windowWidth = ImGui::GetWindowSize().x;
        float cursorX = ImGui::GetCursorPosX();

        char buffer[INPUT_BUFFER_SIZE];
        std::copy(str.begin(), str.begin() + min(str.size(), INPUT_BUFFER_SIZE - 1), buffer);
        buffer[min(str.size(), INPUT_BUFFER_SIZE - 1)] = '\0';

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorX + windowWidth * ITEM_WIDTH_RATIO - LABEL_OFFSET);

        ImGui::PushItemWidth(windowWidth * ITEM_WIDTH_RATIO);
        bool result = ImGui::InputText(("##" + label).c_str(), buffer, INPUT_BUFFER_SIZE, flags);
        ImGui::PopItemWidth();

        if (result)
        {
            str = buffer;
        }

        return result;
    }

    bool DragFloat(const std::string& label, float& value, float min, float max, ImGuiInputTextFlags flags)
    {
        float windowWidth = ImGui::GetWindowSize().x;
        float cursorX = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorX + windowWidth * ITEM_WIDTH_RATIO - LABEL_OFFSET);

        ImGui::PushItemWidth(windowWidth * ITEM_WIDTH_RATIO);
        bool result = ImGui::DragFloat(("##" + label).c_str(), &value, DEFAULT_DRAG_SPEED, min, max, "%.2f", flags);
        ImGui::PopItemWidth();

        return result;
    }

    bool DragAngle3(const std::string& label, float3& value, ImGuiInputTextFlags flags)
    {
        bool changed = DragFloat3(label, value, flags);

        if (changed)
        {
            for (int i = 0; i < 3; ++i)
            {
                value[i] = std::fmod(value[i], ANGLE_MODULO);
                if (value[i] < 0.0f) value[i] += ANGLE_MODULO;
            }
        }

        return changed;
    }

    bool DragFloat3(const std::string& label, float3& value, ImGuiInputTextFlags flags)
    {
        float windowWidth = ImGui::GetWindowSize().x;
        float cursorX = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorX + windowWidth * ITEM_WIDTH_RATIO - LABEL_OFFSET);

        ImGui::PushItemWidth(windowWidth * ITEM_WIDTH_RATIO);
        bool result = ImGui::DragFloat3(("##" + label).c_str(), value.data(), DEFAULT_DRAG_SPEED, DEFAULT_DRAG_MIN, DEFAULT_DRAG_MAX, "%.2f", flags);
        ImGui::PopItemWidth();

        return result;
    }

    bool ColorEdit3(const std::string& label, float3& color, ImGuiInputTextFlags flags)
    {
        float windowWidth = ImGui::GetWindowSize().x;
        float cursorX = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorX + windowWidth * ITEM_WIDTH_RATIO - LABEL_OFFSET);

        ImGui::PushItemWidth(windowWidth * ITEM_WIDTH_RATIO);
        bool result = ImGui::ColorEdit3(("##" + label).c_str(), color.data(), flags);
        ImGui::PopItemWidth();

        return result;
    }
}