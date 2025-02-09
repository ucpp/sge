Texture2D<float4> g_LightingResult : register(t0);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct BrightOutput
{
    float4 color : SV_Target0;
};

float luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

BrightOutput main(PixelInput input)
{
    BrightOutput output;

    float3 color = g_LightingResult.Sample(sampleWrap, input.texCoords).rgb;
    float brightness = luminance(color);

    float threshold = 0.8f;
    output.color = float4(color * step(threshold, brightness), 1.0f);

    return output;
}