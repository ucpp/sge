static const uint MAX_POINT_LIGHTS = 2;
static const uint MAX_SPOT_LIGHTS = 1;

struct DirectionalLight
{
    float3 direction;
    float  intensity;
    float3 color;
};

struct PointLight
{
    float3 position;
    float  intensity;
    float3 color;
};

struct SpotLight
{
    float3 position;
    float  intensity;
    float3 direction;
    float  innerConeCos;
    float3 color;
    float  outerConeCos;
};

cbuffer SceneData : register(b0)
{
    DirectionalLight directionalLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight  spotLights[MAX_SPOT_LIGHTS];
    float3 cameraPosition;
    float  fogStrength;
    float3 fogColor;
    float  fogStart;
    float  fogEnd;
    float  fogDensity;
    float  zNear;
    float  zFar;
    matrix invViewProj;
};