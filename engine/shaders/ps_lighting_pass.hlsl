#include "scene_data.hlsl"

Texture2D<float4> g_AlbedoMetallic : register(t0);  // Albedo + Metallic
Texture2D<float4> g_NormalRoughness : register(t1); // Normal + Roughness
Texture2D<float> g_Depth : register(t2);            // Depth

SamplerState sampleWrap : register(s0);

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

float3 calculatePointLight(float3 worldPos, float3 normal, float3 albedo, PointLight light)
{
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    float diff = max(dot(normal, lightDir), 0.0);

    return albedo * light.color * diff * light.intensity * attenuation;
}

float3 calculateSpotLight(float3 worldPos, float3 normal, float3 albedo, SpotLight light)
{
    float3 lightDir = normalize(light.position - worldPos);
    float distance = length(light.position - worldPos);

    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

    float3 spotDirection = normalize(-light.direction);
    float theta = dot(lightDir, spotDirection);
    float epsilon = light.innerConeCos - light.outerConeCos;
    float intensity = smoothstep(light.outerConeCos, light.innerConeCos, theta);

    float diff = max(dot(normal, lightDir), 0.0);

    return albedo * light.color * diff * light.intensity * attenuation * intensity;
}

float3 ReconstructWorldPos(float2 uv, float depth, float4x4 invViewProj, float zNear, float zFar)
{
    float zLinear = zNear * zFar / (zFar + depth * (zNear - zFar));
    float4 clipPos = float4(uv * 2.0 - 1.0, zLinear, 1.0);
    float4 worldPos = mul(invViewProj, clipPos);
    worldPos /= worldPos.w;

    return worldPos.xyz;
}

LightingOutput main(PixelInput input)
{
    LightingOutput output;

    float4 albedoMetallic = g_AlbedoMetallic.Sample(sampleWrap, input.texCoords);
    float4 normalRoughness = g_NormalRoughness.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);

    float3 albedo = albedoMetallic.rgb;
    float metallic = albedoMetallic.a;
    float3 normal = normalize(normalRoughness.xyz * 2.0f - 1.0f);
    float roughness = normalRoughness.w;

    float3 worldPos = ReconstructWorldPos(input.texCoords, depth, invViewProj, zNear, zFar);

    float3 finalColor = calculateDirectionalLight(normal, albedo, directionalLight);
    for (uint i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        finalColor += calculatePointLight(worldPos, normal, albedo, pointLights[i]);
    }

    output.color = float4(finalColor, 1.0f);
    return output;
}
