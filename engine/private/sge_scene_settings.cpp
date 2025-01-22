#include "sge_scene_settings.h"

namespace SGE
{
    void SceneObjectBase::ToJson(nlohmann::json& j) const
    {
        j = nlohmann::json
        {
            {"name", name},
            {"type", static_cast<int>(type)}
        };
    }

    void SceneObjectBase::FromJson(const nlohmann::json& j)
    {
        j.at("name").get_to(name);
        int32 typeInt {};
        j.at("type").get_to(typeInt);
        type = static_cast<ObjectType>(typeInt);
    }

    void SceneObjectBase::DrawEditor()
    {
        InputTextStdString("Name:", name);
    }

    void TransformData::ToJson(nlohmann::json& j) const
    {
        SceneObjectBase::ToJson(j);
        j["position"] = {position[0], position[1], position[2]};
        j["rotation"] = {rotation[0], rotation[1], rotation[2]};
        j["scale"] = {scale[0], scale[1], scale[2]};
    }

    void TransformData::FromJson(const nlohmann::json& j)
    {
        SceneObjectBase::FromJson(j);
        ParseVector3(j, "position", position);
        ParseVector3(j, "rotation", rotation);
        ParseVector3(j, "scale", scale);
    }

    void TransformData::DrawEditor()
    {
        SceneObjectBase::DrawEditor();
        DragFloat3("Position:", position);
        DragFloat3("Rotation:", rotation);
        DragFloat3("Scale:", scale);
    }

    CameraData::CameraData()
    {
        type = ObjectType::Camera;
    }

    void CameraData::ToJson(nlohmann::json& j) const
    {
        TransformData::ToJson(j);
        j["fov"] = fov;
        j["near_plane"] = nearPlane;
        j["far_plane"] = farPlane;
    }

    void CameraData::FromJson(const nlohmann::json& j)
    {
        TransformData::FromJson(j);
        j.at("fov").get_to(fov);
        j.at("near_plane").get_to(nearPlane);
        j.at("far_plane").get_to(farPlane);
    }

    void CameraData::DrawEditor()
    {
        TransformData::DrawEditor();
        DragFloat("FOV:", &fov);
        DragFloat("Near Plane:", &nearPlane);
        DragFloat("Far Plane:", &farPlane);
    }

    MeshData::MeshData()
    {
        type = ObjectType::Mesh;
    }

    void MeshData::ToJson(nlohmann::json &j) const
    {
        TransformData::ToJson(j);
        j["asset_id"] = assetId;
        j["material_id"] = materialId;
    }

    void MeshData::FromJson(const nlohmann::json &j)
    {
        TransformData::FromJson(j);
        j.at("asset_id").get_to(assetId);
        j.at("material_id").get_to(materialId);
    }

    void MeshData::DrawEditor()
    {
        TransformData::DrawEditor();
        InputTextStdString("Asset ID:", assetId);
        InputTextStdString("Material ID:", materialId);
    }
    
    PointLightData::PointLightData()
    {
        type = ObjectType::PointLight;
    }

    void PointLightData::ToJson(nlohmann::json& j) const
    {
        SceneObjectBase::ToJson(j);
        j["position"] = {position[0], position[1], position[2]};
        j["color"] = {color[0], color[1], color[2]};
        j["intensity"] = intensity;
    }

    void PointLightData::FromJson(const nlohmann::json& j)
    {
        SceneObjectBase::FromJson(j);
        ParseVector3(j, "position", position);
        ParseVector3(j, "color", color);
        j.at("intensity").get_to(intensity);
    }

    void PointLightData::DrawEditor()
    {
        SceneObjectBase::DrawEditor();
        DragFloat3("Position:", position);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", &intensity);
    }

    DirectionalLightData::DirectionalLightData()
    {
        type = ObjectType::DirectionalLight;
    }

    void DirectionalLightData::ToJson(nlohmann::json& j) const
    {
        SceneObjectBase::ToJson(j);
        j["direction"] = {direction[0], direction[1], direction[2]};
        j["color"] = {color[0], color[1], color[2]};
        j["intensity"] = intensity;
    }

    void DirectionalLightData::FromJson(const nlohmann::json& j)
    {
        SceneObjectBase::FromJson(j);
        ParseVector3(j, "direction", direction);
        ParseVector3(j, "color", color);
        j.at("intensity").get_to(intensity);
    }

    void DirectionalLightData::DrawEditor()
    {
        SceneObjectBase::DrawEditor();
        DragFloat3("Direction:", direction);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", &intensity);
    }

    void to_json(nlohmann::json& j, const SceneSettings& settings)
    {
        nlohmann::json objectsJson = nlohmann::json::array();
        for (const auto& obj : settings.objects)
        {
            nlohmann::json objJson;
            obj->ToJson(objJson);
            objectsJson.push_back(objJson);
        }
        j["objects"] = objectsJson;
    }

    void from_json(const nlohmann::json& j, SceneSettings& settings)
    {
        settings.objects.clear();
        if (j.contains("objects"))
        {
            for (const auto& objJson : j.at("objects"))
            {
                int32_t typeInt;
                objJson.at("type").get_to(typeInt);
                auto type = static_cast<ObjectType>(typeInt);

                auto obj = CreateObject(type);
                obj->FromJson(objJson);
                settings.objects.push_back(std::move(obj));
            }
        }
    }

    std::unique_ptr<SceneObjectBase> CreateObject(ObjectType type)
    {
        switch (type)
        {
            case ObjectType::Camera:
                return std::make_unique<CameraData>();
            case ObjectType::Mesh:
                return std::make_unique<MeshData>();
            case ObjectType::PointLight:
                return std::make_unique<PointLightData>();
            case ObjectType::DirectionalLight:
                return std::make_unique<DirectionalLightData>();
            default:
                throw std::runtime_error("Unknown object type");
        }
    }

    void ParseVector3(const nlohmann::json& j, const std::string& key, float* target)
    {
        if (j.contains(key) && j[key].is_array() && j[key].size() == 3)
        {
            auto vec = j[key].get<std::vector<float>>();
            std::copy(vec.begin(), vec.end(), target);
        }
    }
    
    bool InputTextStdString(const std::string& label, std::string& str, ImGuiInputTextFlags flags)
    {
        float width = ImGui::GetWindowSize().x;

        char buffer[1024];
        std::copy(str.begin(), str.begin() + min(str.size(), sizeof(buffer) - 1), buffer);
        buffer[min(str.size(), sizeof(buffer) - 1)] = '\0';

        float x = ImGui::GetCursorPosX();
        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(x + width / 2 - 16);

        ImGui::PushItemWidth(width / 2);
        bool result = ImGui::InputText(("##" + label).c_str(), buffer, sizeof(buffer), flags);
        ImGui::PopItemWidth();

        if (result)
        {
            str = buffer;
        }

        return result;
    }

    bool DragFloat3(const std::string& label, float* values, ImGuiInputTextFlags flags)
    {
        float width = ImGui::GetWindowSize().x;
        float x = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(x + width / 2 - 16);

        ImGui::PushItemWidth(width / 2);
        bool result = ImGui::DragFloat3(("##" + label).c_str(), values, 0.1f, -FLT_MAX, FLT_MAX, "%.1f", flags);
        ImGui::PopItemWidth();

        return result;
    }

    bool ColorEdit3(const std::string& label, float* color, ImGuiInputTextFlags flags)
    {
        float width = ImGui::GetWindowSize().x;
        float x = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(x + width / 2 - 16);

        ImGui::PushItemWidth(width / 2);
        bool result = ImGui::ColorEdit3(("##" + label).c_str(), color, flags);
        ImGui::PopItemWidth();

        return result;
    }

    bool DragFloat(const std::string& label, float* value, ImGuiInputTextFlags flags)
    {
        float width = ImGui::GetWindowSize().x;
        float x = ImGui::GetCursorPosX();

        ImGui::Text(label.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(x + width / 2 - 16);

        ImGui::PushItemWidth(width / 2);
        bool result = ImGui::DragFloat(("##" + label).c_str(), value, 0.1f, -FLT_MAX, FLT_MAX, "%.1f", flags);
        ImGui::PopItemWidth();

        return result;
    }
}