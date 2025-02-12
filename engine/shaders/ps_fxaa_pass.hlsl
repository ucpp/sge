Texture2D<float4> g_targetResult : register(t0);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

float luminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

float4 main(PixelInput input) : SV_Target
{
    uint textureWidth, textureHeight;
    g_targetResult.GetDimensions(textureWidth, textureHeight);
    float2 texelSize = float2(1.0f / textureWidth, 1.0f / textureHeight);

    float3 colorCenter = g_targetResult.Sample(sampleWrap, input.texCoords).rgb;
    
    float3 colorN = g_targetResult.Sample(sampleWrap, input.texCoords + float2(0.0f, texelSize.y)).rgb;
    float3 colorS = g_targetResult.Sample(sampleWrap, input.texCoords - float2(0.0f, texelSize.y)).rgb;
    float3 colorE = g_targetResult.Sample(sampleWrap, input.texCoords + float2(texelSize.x, 0.0f)).rgb;
    float3 colorW = g_targetResult.Sample(sampleWrap, input.texCoords - float2(texelSize.x, 0.0f)).rgb;

    float lumaCenter = luminance(colorCenter);
    float lumaN = luminance(colorN);
    float lumaS = luminance(colorS);
    float lumaE = luminance(colorE);
    float lumaW = luminance(colorW);

    float lumaMin = min(lumaCenter, min(min(lumaN, lumaS), min(lumaE, lumaW)));
    float lumaMax = max(lumaCenter, max(max(lumaN, lumaS), max(lumaE, lumaW)));
    float lumaRange = lumaMax - lumaMin;

    float edgeStrength = saturate((lumaRange - 0.2f) * 5.0f) * step(0.1f, lumaRange);
    float3 blendedColor = lerp(colorCenter, (colorN + colorS + colorE + colorW) * 0.25f, edgeStrength);
    
    return float4(blendedColor, 1.0f);
}
