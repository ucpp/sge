#ifndef _SGE_SCENE_SETTINGS_H_
#define _SGE_SCENE_SETTINGS_H_

#include "pch.h"
#include "json.hpp"

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

    public:
        std::string name;
        ObjectType type;
    };

    class TransformData : public SceneObjectBase
    {
    public:
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;

    public:
        float position[3] = {0.0f, 0.0f, 0.0f};
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float scale[3] = {0.0f, 0.0f, 0.0f};
    };

    class CameraData : public TransformData
    {
    public:
        CameraData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;

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
    
    public:
        std::string path;
        std::string materialId;
    };

    class PointLightData : public SceneObjectBase
    {
    public:
        PointLightData();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;

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
    
    public:
        float direction[3];
        float color[3];
        float intensity;
    };

    class SceneSettings
    {
    public:
        std::vector<std::unique_ptr<SceneObjectBase>> objects;
    };

    void to_json(nlohmann::json& j, const SceneSettings& settings);
    void from_json(const nlohmann::json& j, SceneSettings& settings);

    std::unique_ptr<SceneObjectBase> CreateObject(ObjectType type);
    void ParseVector3(const nlohmann::json& j, const std::string& key, float* target);
}

#endif // !_SGE_SCENE_SETTINGS_H_