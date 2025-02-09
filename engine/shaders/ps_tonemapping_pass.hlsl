Texture2D<float4> g_LightingResult : register(t0);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct TonemappingOutput
{
    float4 color : SV_Target0;
};

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

TonemappingOutput main(PixelInput input) : SV_Target
{
    TonemappingOutput output;

    float3 color = g_LightingResult.Sample(sampleWrap, input.texCoords).rgb;

    color = ACESFilm(color);
    color = pow(color, 1.0 / 2.2);

    float exposure = 1.1;
    color *= exposure;
    
    output.color = float4(color, 1.0f);
    return output;
}