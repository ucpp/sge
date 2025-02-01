#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicMap : register(t2);
Texture2D roughnessMap : register(t3);

SamplerState sampleWrap : register(s0);

struct GBufferOutput
{
    float4 AlbedoMetallic : SV_Target0;  // RGB: Albedo, A: Metallic
    float4 NormalRoughness : SV_Target1; // RGB: Normal, A: Roughness
};

float3 EncodeNormalToRGB(float3 normal)
{
    return 0.5f * (normal + 1.0f);
}

GBufferOutput main(PixelInput input)
{
    GBufferOutput output;

    float3 albedo = diffuseMap.Sample(sampleWrap, input.texCoords).rgb;
    float metallic = metallicMap.Sample(sampleWrap, input.texCoords).r;
    float3 normalMapValue = normalMap.Sample(sampleWrap, input.texCoords).xyz * 2.0f - 1.0f;
    float roughness = roughnessMap.Sample(sampleWrap, input.texCoords).r;

    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = normalize(mul(normalMapValue, TBN));

    output.AlbedoMetallic = float4(albedo, metallic);
    output.NormalRoughness = float4(EncodeNormalToRGB(normal), roughness);

    return output;
}