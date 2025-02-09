#include "scene_data.hlsl"

Texture2D<float4> g_NormalRoughness : register(t0);
Texture2D<float> g_Depth : register(t1);
SamplerState sampleWrap : register(s0);

static const int SSAO_SAMPLE_COUNT = 16;
static const float3 SSAO_Samples[SSAO_SAMPLE_COUNT] = {
    float3(0.04977, -0.04471, 0.04996), float3(0.01457, 0.01653, 0.00224),
    float3(-0.04065, -0.01937, 0.03193), float3(0.01378, -0.09158, 0.04092),
    float3(0.05599, 0.05979, 0.05766), float3(0.09227, 0.04428, 0.01545),
    float3(-0.00204, -0.0544, 0.06674), float3(-0.00033, -0.00019, 0.00037),
    float3(0.05004, -0.04665, 0.02538), float3(0.03813, 0.0314, 0.03287),
    float3(-0.03188, 0.02046, 0.02251), float3(0.0557, -0.03697, 0.05449),
    float3(0.05737, -0.02254, 0.07554), float3(-0.01609, -0.00377, 0.05547),
    float3(0.03902, 0.00787, 0.04254), float3(-0.04938, 0.01132, 0.01015)
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct SSAOOutput
{
    float4 color : SV_Target0;
};

float3 ReconstructWorldPosition(float2 uv, float depth)
{
    float2 clipSpaceXY = uv * 2.0 - 1.0;
    clipSpaceXY.y = -clipSpaceXY.y;
    
    float4 clipSpacePosition = float4(clipSpaceXY, depth, 1.0);
    float4 cameraSpacePosition = mul(clipSpacePosition, invProj);
    cameraSpacePosition.xyzw /= cameraSpacePosition.w;

    float4 worldSpacePosition = mul(cameraSpacePosition, invView);

    return worldSpacePosition.xyz;
}

float ComputeSSAO(float3 worldPos, float3 normal, float2 texCoords)
{
    float occlusion = 0.0;
    const float radius = 0.3;

    for (int i = 0; i < SSAO_SAMPLE_COUNT; i++)
    {
        float3 sampleOffset = SSAO_Samples[i] * radius;
        float3 samplePos = worldPos + sampleOffset;

        float4 sampleClipPos = mul(float4(samplePos, 1.0), viewProj);
        sampleClipPos.xy /= sampleClipPos.w;
        float2 sampleUV = sampleClipPos.xy * 0.5 + 0.5;
        sampleUV.y = 1.0 - sampleUV.y;

        float sampleDepth = g_Depth.Sample(sampleWrap, sampleUV).r;
        float3 sampleWorldPos = ReconstructWorldPosition(sampleUV, sampleDepth);

        float3 toSample = normalize(sampleWorldPos - worldPos);
        float normalFactor = max(dot(normal, toSample), 0.0);

        float rangeCheck = smoothstep(0.0, 1.0, radius / length(worldPos - sampleWorldPos));
        float bias = 0.025 * radius;

        occlusion += (sampleWorldPos.z <= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck * normalFactor;
    }

    return saturate(1.0 - (occlusion / SSAO_SAMPLE_COUNT) * 1.2);
}

SSAOOutput main(PixelInput input)
{
    SSAOOutput output;

    float4 normalRoughness = g_NormalRoughness.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);

    float3 normal = normalize(normalRoughness.xyz * 2.0f - 1.0f);
    float roughness = normalRoughness.w;

    float3 worldPos = ReconstructWorldPosition(input.texCoords, depth);
    
    float ssao = ComputeSSAO(worldPos, normal, input.texCoords);
    ssao = pow(ssao, 1.2);
    output.color = float4(ssao, ssao, ssao, 1.0);

    return output;
}
