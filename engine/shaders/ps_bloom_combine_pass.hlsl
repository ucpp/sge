Texture2D<float4> g_LightingResult : register(t0);
Texture2D<float4> g_BlurredTexture : register(t1);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct BloomOutput
{
    float4 color : SV_Target0;
};

BloomOutput main(PixelInput input)
{
    BloomOutput output;

    float3 originalColor = g_LightingResult.Sample(sampleWrap, input.texCoords).rgb;
    float3 blurredColor = g_BlurredTexture.Sample(sampleWrap, input.texCoords).rgb;

    float3 finalColor = originalColor + blurredColor;

    output.color = float4(finalColor, 1.0);
    return output;
}