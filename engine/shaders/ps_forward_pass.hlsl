#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"
#include "brdf.hlsl"
#include "shadows.hlsl"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D<float> g_ShadowMap : register(t4);

SamplerState sampleWrap : register(s0);
SamplerState sampleClamp : register(s1);

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
    
    return float4(finalColor, 1.0);
}
