Texture2D<float4> g_BrightTexture : register(t0);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct BlurOutput
{
    float4 color : SV_Target0;
};

static const float weight[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

BlurOutput main(PixelInput input)
{
    BlurOutput output;

    float2 textureSize;
    g_BrightTexture.GetDimensions(textureSize.x, textureSize.y);  // Or GetDimensions(0, textureSize.x, textureSize.y) for mip level 0

    float2 texOffset = float2(1.0f / textureSize.x, 1.0f / textureSize.y);

    float3 result = g_BrightTexture.Sample(sampleWrap, input.texCoords).rgb * weight[0];

    for (int i = 1; i < 5; ++i)
    {
        result += g_BrightTexture.Sample(sampleWrap, input.texCoords + float2(texOffset.x * i, 0.0f)).rgb * weight[i];
        result += g_BrightTexture.Sample(sampleWrap, input.texCoords - float2(texOffset.x * i, 0.0f)).rgb * weight[i];
    }

    output.color = float4(result, 1.0f);
    return output;
}