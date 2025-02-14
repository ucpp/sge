#include "scene_data.hlsl"

TextureCube skyboxTexture : register(t0);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    float3 color = skyboxTexture.Sample(sampleWrap, input.texCoord).rgb;
    return float4(color, 1.0f);
}