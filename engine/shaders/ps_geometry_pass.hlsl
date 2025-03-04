#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"

Texture2D<float4> diffuseMap : register(t0);
Texture2D<float4> normalMap : register(t1);
Texture2D<float4> metallicMap : register(t2);
Texture2D<float4> roughnessMap : register(t3);

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

    float2 uv = float2(input.texCoords.x, 1.0f - input.texCoords.y) * tilingUV;

    float3 albedo = diffuseMap.Sample(sampleWrap, uv).rgb;
    float  metallic = metallicMap.Sample(sampleWrap, uv).r;
    float3 normalMapValue = normalMap.Sample(sampleWrap, uv).xyz * 2.0f - 1.0f;
    float  roughness = roughnessMap.Sample(sampleWrap, uv).r;

    float2 tangentBitangent = normalMapValue.xy;
    float  normalLength = sqrt(saturate(1.0f - dot(tangentBitangent, tangentBitangent)));

    float3 meshNormal = normalize(input.normal);
    float3 meshTangent = normalize(input.tangent);
    float3 meshBitangent = normalize(cross(meshTangent, meshNormal));

    float3 worldNormal = normalize(meshTangent * tangentBitangent.x + meshBitangent * tangentBitangent.y + meshNormal * normalLength);

    output.AlbedoMetallic = float4(albedo, metallic);
    output.NormalRoughness = float4(EncodeNormalToRGB(worldNormal), roughness);

    return output;
}