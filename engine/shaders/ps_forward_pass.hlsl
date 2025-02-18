#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"
#include "brdf.hlsl"
#include "shadows.hlsl"

Texture2D<float4> diffuseMap : register(t0);
Texture2D<float4> normalMap : register(t1);
Texture2D<float4> metallicMap : register(t2);
Texture2D<float4> roughnessMap : register(t3);
Texture2D<float> g_ShadowMap : register(t4);
Texture2D<float4> g_SSAO : register(t5);

SamplerState sampleWrap : register(s0);
SamplerState sampleClamp : register(s1);

float2 WorldToScreenUV(float3 worldPosition)
{
    float4 clipPos = mul(float4(worldPosition, 1.0), viewProj);
    float3 ndc = clipPos.xyz / clipPos.w;

    float2 screenUV = ndc.xy * 0.5 + 0.5;
    screenUV.y = 1.0 - screenUV.y;

    return screenUV;
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 albedo = diffuseMap.Sample(sampleWrap, input.texCoords).rgb;
    float3 normalMapValue = normalMap.Sample(sampleWrap, input.texCoords).xyz * 2.0f - 1.0f;
    float metallic = metallicMap.Sample(sampleWrap, input.texCoords).r;
    float roughness = roughnessMap.Sample(sampleWrap, input.texCoords).r;
    
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = normalize(mul(normalMapValue, TBN));
    float3 viewDir = normalize(cameraPosition - input.worldPosition);

    float shadow = CalculateShadow(g_ShadowMap, sampleClamp, input.worldPosition);
    float shadowFactor = 1.0 - shadow;
    
    float3 finalColor = CalculateDirectionalLight(normal, albedo, metallic, roughness, viewDir, directionalLight) * shadowFactor;
    for (uint i = 0; i < activePointLightsCount; ++i)
    {
        finalColor += CalculatePointLight(input.worldPosition, normal, albedo, metallic, roughness, viewDir, pointLights[i]);
    }
    for (uint i = 0; i < activeSpotLightsCount; ++i)
    {
        finalColor += CalculateSpotLight(input.worldPosition, normal, albedo, metallic, roughness, viewDir, spotLights[i]);
    }

    float2 screenUV = WorldToScreenUV(input.worldPosition);
    float ssao = lerp(1.0, g_SSAO.Sample(sampleWrap, screenUV).r, 0.9f);
    finalColor *= ssao;
    
    return float4(finalColor, 1.0);
}
