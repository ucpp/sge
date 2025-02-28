#ifndef _SGE_DATA_STRUCTURES_H_
#define _SGE_DATA_STRUCTURES_H_

#include "pch.h"
#include <unordered_set>

namespace SGE
{
    struct alignas(16) Vertex
    {
        float3 position;
        float3 normal;
        float2 texCoords;
        float3 tangent;
        float3 bitangent;
        float  boneWeights[4];
        int32  boneIndices[4]; 
    };
    static_assert(alignof(Vertex) == 16, "Vertex structure alignment mismatch");

    struct alignas(16) TransformBuffer
    {
        float4x4 model;
        float4x4 view;
        float4x4 projection;
        float4x4 boneTransforms[100];
        bool isAnimated;
        char padding[15];
    };
    static_assert(alignof(TransformBuffer) == 16, "TransformBuffer structure alignment mismatch");
    static_assert(sizeof(TransformBuffer) == (64 * 103) + 16, "TransformBuffer size mismatch");

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
        float4x4 viewProjSky{};
        float4x4 lightView{};
        float4x4 lightProj{};
        uint32 activePointLightsCount{};
        uint32 activeSpotLightsCount{};
    };
    static_assert(alignof(FrameData) == 16, "FrameData structure alignment mismatch");

    enum class RTargetType
    {
        AlbedoMetallic   = 0,
        NormalRoughness  = 1,
        LightingBuffer   = 2,
        BrightnessBuffer = 3,
        BlurBuffer       = 4,
        BloomBuffer      = 5,
        SSAOBuffer       = 6,
        ToneMapping      = 7
    };

    enum class RenderTechnique
    {
        Forward   = 0,
        Deferred  = 1
    };
    
    enum class ObjectType
    {
        Camera           = 0,
        DirectionalLight = 1,
        Skybox           = 2,
        Model            = 3,
        AnimatedModel    = 4,
        PointLight       = 5
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

    class AnimatedModelData : public ModelData 
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;

        std::unordered_map<std::string, int> boneLayers;
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

    class SkyboxData : public ObjectDataBase
    {
    public:
        void DrawEditor() override;
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;
    
    public:
        std::string cubemapId;
    };

    enum class AssetType
    {
        Model         = 0,
        AnimatedModel = 1,
        Material      = 2,
        Light         = 3,
        Cubemap       = 4
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

    class AnimatedModelAssetData : public ModelAssetData {};

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

    class CubemapAssetData : public AssetDataBase
    {
    public:
        void ToJson(njson& data) override;
        void FromJson(const njson& data) override;
        std::array<std::string, 6> GetPaths() const;
    
    public:
        std::string back;
        std::string bottom;
        std::string front;
        std::string left;
        std::string right;
        std::string top;
    };

    class AssetsData
    {
    public:
        const MaterialAssetData& GetMaterial(const std::string& id) const;
        const ModelAssetData& GetModel(const std::string& id) const;
        const AnimatedModelAssetData& GetAnimModel(const std::string& id) const;
        const CubemapAssetData& GetCubemap(const std::string& id) const;

    public:
        std::map<AssetType, std::unordered_map<std::string, std::unique_ptr<AssetDataBase>>> assets;
    };

    class SceneData
    {
    public:
        CameraData* GetCameraData();
        DirectionalLightData* GetDirectionalLight();
        SkyboxData* GetSkyboxData();

    public:
        std::map<ObjectType, std::vector<std::unique_ptr<ObjectDataBase>>> objects;

    private:
        CameraData* m_mainCamera = nullptr;
        DirectionalLightData* m_directionalLight = nullptr;
        SkyboxData* m_skyboxData = nullptr;
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

    struct RenderPassData
    {
        std::string name;
        std::vector<std::string> input;
        std::vector<std::string> output;
    };

    class RenderData
    {
    public:
        bool vSync = true;
        std::string finalRender;
        RenderTechnique technique = RenderTechnique::Deferred;
    
        std::vector<RenderPassData> forwardPasses;
        std::vector<RenderPassData> deferredPasses;
    
        std::unordered_set<std::string> GetAllPassNames() const;
        std::unordered_set<std::string> GetAllOutputs() const;
    
        const std::vector<std::string>& GetForwardOutputs() const;
        const std::vector<std::string>& GetDeferredOutputs() const;
    
        const std::vector<const char*>& GetForwardOutputsData() const;
        const std::vector<const char*>& GetDeferredOutputsData() const;
    
        uint32 GetForwardOutputsCount() const;
        uint32 GetDeferredOutputsCount() const;
    
    private:
        void BuildForwardOutputsCache() const;
        void BuildDeferredOutputsCache() const;
    
    private:
        mutable std::vector<std::string> cachedForwardOutputs;
        mutable std::vector<std::string> cachedDeferredOutputs;
        mutable std::vector<const char*> cachedForwardOutputPtrs;
        mutable std::vector<const char*> cachedDeferredOutputPtrs;
        mutable bool forwardOutputsCached = false;
        mutable bool deferredOutputsCached = false;
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

    void to_json(njson& data, const RenderPassData& pass);
    void from_json(const njson& data, RenderPassData& pass);
}

#endif // !_SGE_DATA_STRUCTURES_H_