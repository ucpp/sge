Texture2D<float4> g_AlbedoMetallic : register(t0);  // Albedo + Metallic
Texture2D<float4> g_NormalRoughness : register(t1); // Normal + Roughness
Texture2D<float> g_Depth : register(t2);           // Depth

SamplerState sampleWrap : register(s0);

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
    float2 texCoords : TEXCOORD0;
};

struct LightingOutput
{
    float4 color : SV_Target0;
};

float3 calculateDirectionalLight(float3 normal, float3 albedo, DirectionalLight light)
{
    float3 lightDir = normalize(-light.direction); 
    float diff = max(dot(normal, lightDir), 0.0);
    return albedo * light.color * diff * light.intensity;
}

LightingOutput main(PixelInput input)
{
    LightingOutput output;

    float4 albedoMetallic = g_AlbedoMetallic.Sample(sampleWrap, input.texCoords);
    float4 normalRoughness = g_NormalRoughness.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);

    output.color = albedoMetallic;

    return output;
}
