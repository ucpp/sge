float CalculateShadowPCF(Texture2D<float> shadowMap, SamplerState samplerState, float3 shadowCoord, float bias)
{
    float shadow = 0.0;
    float2 textureSize;
    shadowMap.GetDimensions(textureSize.x, textureSize.y);
    float2 texelSize = 1.0 / textureSize;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            float2 sampleCoord = shadowCoord.xy + offset;

            float inBounds = step(0.0, sampleCoord.x) * step(sampleCoord.x, 1.0) * step(0.0, sampleCoord.y) * step(sampleCoord.y, 1.0);

            float shadowMapDepth = shadowMap.Sample(samplerState, sampleCoord);
            shadow += inBounds * (shadowCoord.z - bias > shadowMapDepth ? 1.0 : 0.0);
        }
    }

    return shadow / 9.0;
}

float CalculateShadow(Texture2D<float> shadowMap, SamplerState samplerState, float3 worldPos)
{
    float4 shadowCoord = mul(float4(worldPos.xyz, 1.0), mul(lightView, lightProj));
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.y = -shadowCoord.y;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;

    float inBounds = step(0.0, shadowCoord.x) * step(shadowCoord.x, 1.0) *
                     step(0.0, shadowCoord.y) * step(shadowCoord.y, 1.0) *
                     step(0.0, shadowCoord.z) * step(shadowCoord.z, 1.0);

    float bias = 0.005;
    return inBounds * CalculateShadowPCF(shadowMap, samplerState, shadowCoord.xyz, bias);
}