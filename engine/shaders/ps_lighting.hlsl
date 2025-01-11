
Texture2D<float4> g_AlbedoMetallic : register(t0); // Albedo + Metallic
Texture2D<float4> g_NormalRoughness : register(t1); // Normal + Roughness
Texture2D<float> g_Depth : register(t2); // Depth

SamplerState sampleWrap : register(s0);

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

cbuffer SceneData : register(b0)
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    float3 cameraPosition;
    float fogStrength;
    float3 fogColor;
    float fogStart;
    float fogEnd;
    float fogDensity;
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

struct LightingOutput
{
    float4 color : SV_Target0;
};

LightingOutput main(PixelInput input)
{
    LightingOutput output;

    // Sample the G-Buffer textures
    float4 albedoMetallic = g_AlbedoMetallic.Sample(sampleWrap, input.texCoords);
    float4 normalRoughness = g_NormalRoughness.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);

    float3 normal = normalize(normalRoughness.rgb * 2.0f - 1.0f);

    float3 lightDir = normalize(directionalLight.direction);
    float diff = max(dot(normal, -lightDir), 0.0f);
    float3 diffuse = diff * directionalLight.color * albedoMetallic.rgb;

    output.color = float4(diffuse, 1.0f);

    return output;
}
