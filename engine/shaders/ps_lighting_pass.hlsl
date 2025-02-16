#include "scene_data.hlsl"
#include "brdf.hlsl"
#include "shadows.hlsl"

Texture2D<float4> g_AlbedoMetallic : register(t0);
Texture2D<float4> g_NormalRoughness : register(t1);
Texture2D<float4> g_SSAO : register(t2);
Texture2D<float> g_Depth : register(t3);
Texture2D<float> g_ShadowMap : register(t4);
SamplerState sampleWrap : register(s0);
SamplerState sampleClamp : register(s1);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct LightingOutput
{
    float4 color : SV_Target0;
};

float3 ReconstructWorldPosition(float2 uv, float depth)
{
    float2 clipSpaceXY = uv * 2.0 - 1.0;
    clipSpaceXY.y = -clipSpaceXY.y;
    
    float4 clipSpacePosition = float4(clipSpaceXY, depth, 1.0);
    float4 cameraSpacePosition = mul(clipSpacePosition, invProj);
    cameraSpacePosition.xyzw /= cameraSpacePosition.w;

    float4 worldSpacePosition = mul(cameraSpacePosition, invView);

    return lerp(worldSpacePosition.xyz, float3(0.0, 0.0, 1e5), step(1.0, depth));
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

    float3 worldPos = ReconstructWorldPosition(input.texCoords, depth);
    float3 viewDir = normalize(cameraPosition - worldPos);

    float shadow = CalculateShadow(g_ShadowMap, sampleClamp, worldPos);
    float shadowFactor = 1.0 - shadow;
    float3 finalColor = CalculateDirectionalLight(normal, albedo, metallic, roughness, viewDir, directionalLight) * shadowFactor;

    for (uint i = 0; i < activePointLightsCount; ++i)
    {
        finalColor += CalculatePointLight(worldPos, normal, albedo, metallic, roughness, viewDir, pointLights[i]);
    }

    float ssao = lerp(1.0, g_SSAO.Sample(sampleWrap, input.texCoords).r, 0.9f);
    finalColor *= ssao;

    output.color = float4(finalColor, 1.0f);

    return output;
}