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

static const float2 offsets[9] = 
{
    float2( 0.0,  0.0), float2( 1.0,  0.0), float2(-1.0,  0.0),
    float2( 0.0,  1.0), float2( 0.0, -1.0), float2( 0.7,  0.7), 
    float2(-0.7, -0.7), float2( 0.7, -0.7), float2(-0.7,  0.7)
};

static const float weights[9] =
{
    0.25f, 0.125f, 0.125f,
    0.125f, 0.125f, 0.0625f,
    0.0625f, 0.0625f, 0.0625f
};

BlurOutput main(PixelInput input)
{
    BlurOutput output;
    
    float2 textureSize;
    g_BrightTexture.GetDimensions(textureSize.x, textureSize.y);
    float2 texOffset = float2(1.0 / textureSize.x, 1.0 / textureSize.y);

    float3 result = float3(0.0, 0.0, 0.0);
    float blurRadius = 1.5f;
    for (int i = 0; i < 9; ++i)
    {
        result += g_BrightTexture.Sample(sampleWrap, input.texCoords + offsets[i] * texOffset * blurRadius).rgb * weights[i];
    }
    
    output.color = float4(result, 1.0f);
    return output;
}