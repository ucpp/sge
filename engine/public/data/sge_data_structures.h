#ifndef _SGE_DATA_STRUCTURES_H_
#define _SGE_DATA_STRUCTURES_H_

#include "pch.h"

namespace SGE
{
    struct alignas(16) Vertex
    {
        float3 position;
        float3 normal;
        float2 texCoords;
        float3 tangent;
        float3 bitangent;
    };
    static_assert(alignof(Vertex) == 16, "Vertex structure alignment mismatch");

    struct alignas(16) TransformBuffer
    {
        float4x4 model;
        float4x4 view;
        float4x4 projection;
    };
    static_assert(alignof(TransformBuffer) == 16, "TransformBuffer structure alignment mismatch");

    struct alignas(16) DirectionalLight
    {
        float3 direction;
        float  intensity;
        float3 color;
    };
    static_assert(alignof(DirectionalLight) == 16, "DirectionalLight structure alignment mismatch");

    struct alignas(16) PointLight
    {
        float3 position;
        float  intensity;
        float3 color;
        float radius;
    };
    static_assert(alignof(PointLight) == 16, "PointLight structure alignment mismatch");

    struct alignas(16) SpotLight
    {
        float3 position;
        float  intensity;
        float3 direction;
        float  innerConeCos;
        float3 color;
        float  outerConeCos;
    };
    static_assert(alignof(SpotLight) == 16, "SpotLight structure alignment mismatch");

    struct alignas(16) FrameData
    {
        DirectionalLight directionalLight{};
        PointLight       pointLights[MAX_POINT_LIGHTS]{};
        SpotLight        spotLights[MAX_SPOT_LIGHTS]{};
        float3 cameraPosition{};
        float  fogStrength{};
        float3 fogColor{};
        float  fogStart{};
        float  fogEnd{};
        float  fogDensity{};
        float  zNear{};
        float  zFar{};
        float4x4 invProj{};
        float4x4 invView{};
        float4x4 viewProj{};
        uint32 activePointLightsCount{};
        uint32 activeSpotLightsCount{};
    };
    static_assert(alignof(FrameData) == 16, "FrameData structure alignment mismatch");

    enum class ObjectType
    {
        Camera           = 0,
        DirectionalLight = 1,
        Model            = 2,
        PointLight       = 3
    };

    class ObjectDataBase
    {
    public:
        virtual ~ObjectDataBase() = default;
        virtual void DrawEditor();
        virtual void ToJson(njson& data);
        virtual void FromJson(const njson& data);

    public:
        std::string name;
        ObjectType  type;
        bool enabled = true;
    };

    struct Transform
    {
        float3 position;
        float3 rotation;
        float3 scale;
    };

    class TransformData : public ObjectDataBase
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

    public:
        Transform transform;
    };

    class CameraData : public TransformData
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

    public:
        float fov;
        float nearPlane; 
        float farPlane;
        float moveSpeed;
        float sensitivity;
    };

    class ModelData : public TransformData
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;
    
    public:
        std::string assetId;
        std::string materialId;
    };

    class PointLightData : public ObjectDataBase
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

    public:
        float3 position;
        float3 color;
        float  intensity;
        float  radius;
    };

    class DirectionalLightData : public ObjectDataBase
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;
    
    public:
        float3 direction;
        float3 color;
        float  intensity;
    };

    enum class AssetType
    {
        Model     = 0,
        Material  = 1,
        Light     = 2
    };

    class AssetDataBase
    {
    public:
        virtual ~AssetDataBase() = default;
        virtual void ToJson(njson& data);
        virtual void FromJson(const njson& data);

    public:
        std::string name;
        AssetType   type;
    };

    class ModelAssetData : public AssetDataBase
    {
    public:
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

    public:
        std::string path;
    };

    class MaterialAssetData : public AssetDataBase
    {
    public:
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

    public:
        std::string albedoTexturePath;
        std::string metallicTexturePath;
        std::string normalTexturePath;
        std::string roughnessTexturePath;
    };

    class LightAssetData : public AssetDataBase { };

    class AssetsData
    {
    public:
        const MaterialAssetData& GetMaterial(const std::string& id) const;
        const ModelAssetData& GetModel(const std::string& id) const;

    public:
        std::map<AssetType, std::unordered_map<std::string, std::unique_ptr<AssetDataBase>>> assets;
    };

    class SceneData
    {
    public:
        CameraData* GetCameraData();
        DirectionalLightData* GetDirectionalLight();

    public:
        std::map<ObjectType, std::vector<std::unique_ptr<ObjectDataBase>>> objects;

    private:
        CameraData* m_mainCamera = nullptr;
        DirectionalLightData* m_directionalLight = nullptr;
    };

    class WindowData
    {
    public:
        uint32 GetWidth() const;
        uint32 GetHeight() const;

        const char** GetResolutions();
        void UpdateResolutionCStrings();

    public:
        std::string title;
        bool fullscreen;
        uint32 targetFPS;
        int32 selectedResolution;
        std::vector<std::string> resolutions;
        bool isEditorEnable;

        bool isPressedQuit = false;

    private:
        std::vector<const char*> resolutionCStrings;
    };

    class RenderData
    {
    public:
        bool vSync = true;
        bool isMSAAEnabled = true;
        bool isFogEnabled = false;
        bool isDeferredRendering = false;
    };

    class ApplicationData
    {
    public:
        WindowData windowData;
        RenderData renderData;
        AssetsData assetsData;
        SceneData  sceneData;
    };

    inline std::string roundToString(float1 value, int32 precision = 3) noexcept;
    inline float1 roundFromString(const std::string& str, int32 precision = 3) noexcept;

    void to_json(njson& data, float1 value);
    void from_json(const njson& data, float1& value);

    void to_json(njson& data, const float2& vec2);
    void from_json(const njson& data, float2& vec2);

    void to_json(njson& data, const float3& vec3);
    void from_json(const njson& data, float3& vec3);

    void to_json(njson& data, const SceneData& scene);
    void from_json(const njson& data, SceneData& scene);

    void to_json(njson& data, const WindowData& window);
    void from_json(const njson& data, WindowData& window);

    void to_json(njson& data, const RenderData& render);
    void from_json(const njson& data, RenderData& render);

    void to_json(njson& data, const AssetsData& assets);
    void from_json(const njson& data, AssetsData& assets);

    void to_json(njson& data, const ApplicationData& application);
    void from_json(const njson& data, ApplicationData& application);
}

#endif // !_SGE_DATA_STRUCTURES_H_