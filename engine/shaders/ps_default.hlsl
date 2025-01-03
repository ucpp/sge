cbuffer TransformBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float lodLevel;
}

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 color = diffuseMap.SampleLevel(sampleWrap, input.texCoords, lodLevel);
    return color;
}
