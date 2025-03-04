Texture2D<float4> g_final : register(t0);
Texture2D<float> g_Depth : register(t1);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct FinalOutput
{
    float4 color : SV_Target0;
};

float4 main(PixelInput input) : SV_Target
{
    float4 color = g_final.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);
    const float epsilon = 0.0001;
    clip(1.0 - depth - epsilon);
    return color;
}