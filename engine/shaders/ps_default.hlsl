Texture2D diffuseMap : register(t0);

SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 color = diffuseMap.Sample(sampleWrap, input.texCoords);
    return color;
}
