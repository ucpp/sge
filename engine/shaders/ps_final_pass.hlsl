Texture2D<float4> g_final : register(t0);
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
    return g_final.Sample(sampleWrap, input.texCoords);
}