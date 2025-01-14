Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
SamplerState sampleWrap : register(s0);

static const uint MAX_POINT_LIGHTS = 2;
static const uint MAX_SPOT_LIGHTS = 1;

cbuffer TransformBuffer : register(b1)
{
    matrix model;
    matrix view;
    matrix projection;
}

struct DirectionalLight
{
    float3 direction;
    float intensity;
    float3 color;
};

struct PointLight
{
    float3 position;
    float intensity;
    float3 color;
};

struct SpotLight
{
    float3 position;
    float intensity;
    float3 direction;
    float innerConeCos;
    float3 color;
    float outerConeCos;
};

cbuffer SceneData : register(b0)
{
    DirectionalLight directionalLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    float3 cameraPosition;
    float fogStrength;
    float3 fogColor;
    float fogStart;
    float fogEnd;
    float fogDensity;
    float zNear;
    float zFar;
    matrix invViewProj;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct GBufferOutput
{
    float4 AlbedoMetallic : SV_Target0;
    float4 NormalRoughness : SV_Target1;
};

float3 EncodeNormalToRGB(float3 normal)
{
    return 0.5f * (normal + 1.0f);
}

GBufferOutput main(PixelInput input)
{
    GBufferOutput output;

    float3 albedo = diffuseMap.Sample(sampleWrap, input.texCoords).rgb;
    float metallic = specularMap.Sample(sampleWrap, input.texCoords).r;
    float3 normalMapValue = normalMap.Sample(sampleWrap, input.texCoords).xyz * 2.0f - 1.0f;

    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = normalize(mul(normalMapValue, TBN));

    output.AlbedoMetallic = float4(albedo, metallic);
    output.NormalRoughness = float4(EncodeNormalToRGB(normal), 0.0);

    return output;
}