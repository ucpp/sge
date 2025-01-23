#ifndef _SGE_SCENE_SETTINGS_H_
#define _SGE_SCENE_SETTINGS_H_

#include "pch.h"
#include "json.hpp"
#include "imgui.h"

namespace SGE
{
    enum class ObjectType
    {
        Camera           = 0,
        Mesh             = 1,
        PointLight       = 2,
        DirectionalLight = 3
    };

    class SceneObjectBase
    {
    public:
        virtual ~SceneObjectBase() = default;

        virtual void ToJson(nlohmann::json& j) const;
        virtual void FromJson(const nlohmann::json& j);
        virtual void DrawEditor();

    public:
        std::string name;
        ObjectType type;
    };

    class TransformData : public SceneObjectBase
    {
    public:
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
        void DrawEditor() override;

    public:
        float position[3] = {0.0f, 0.0f, 0.0f};
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float scale[3] = {1.0f, 1.0f, 1.0f};
    };

    class CameraData : public TransformData
    {
    public:
        CameraData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
        void DrawEditor() override;

    public:
        float fov;
        float nearPlane; 
        float farPlane;
    };

    class MeshData : public TransformData
    {
    public:
        MeshData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
        void DrawEditor() override;
    
    public:
        std::string assetId;
        std::string materialId;
    };

    class PointLightData : public SceneObjectBase
    {
    public:
        PointLightData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
        void DrawEditor() override;

    public:
        float position[3];
        float color[3];
        float intensity;
    };

    class DirectionalLightData : public SceneObjectBase
    {
    public:
        DirectionalLightData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
        void DrawEditor() override;
    
    public:
        float direction[3];
        float color[3];
        float intensity;
    };

    class SceneSettings
    {
        friend void from_json(const nlohmann::json& j, SceneSettings& settings);
    public:
        CameraData* GetCameraData() const { return m_mainCamera; }

    public:
        std::vector<std::unique_ptr<SceneObjectBase>> objects;

    private:
        CameraData* m_mainCamera = nullptr;
    };

    void to_json(nlohmann::json& j, const SceneSettings& settings);
    void from_json(const nlohmann::json& j, SceneSettings& settings);

    std::unique_ptr<SceneObjectBase> CreateObject(ObjectType type);
    void ParseVector3(const nlohmann::json& j, const std::string& key, float* target);

    bool InputTextStdString(const std::string& label, std::string& str, ImGuiInputTextFlags flags = 0);
    bool DragFloat3(const std::string& label, float* values, ImGuiInputTextFlags flags = 0);
    bool ColorEdit3(const std::string& label, float* color, ImGuiInputTextFlags flags = 0);
    bool DragFloat(const std::string& label, float* value, float min = -FLT_MAX, float max = FLT_MAX, ImGuiInputTextFlags flags = 0);
}

#endif // !_SGE_SCENE_SETTINGS_H_