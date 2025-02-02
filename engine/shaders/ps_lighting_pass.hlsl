#include "scene_data.hlsl"

Texture2D<float4> g_AlbedoMetallic : register(t0);
Texture2D<float4> g_NormalRoughness : register(t1);
Texture2D<float> g_Depth : register(t2);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

struct LightingOutput
{
    float4 color : SV_Target0;
};

static const float PI = 3.14159265359;

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 CalculateLightContribution(float3 albedo, float metallic, float roughness, float3 normal, float3 lightDir, float3 viewDir, float3 lightColor, float intensity)
{
    float3 halfDir = normalize(viewDir + lightDir);
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    float NDF = DistributionGGX(normal, halfDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);

    float3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
    float3 specular = nominator / denominator;

    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);

    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 diffuse = kD * albedo / PI;

    return (diffuse + specular) * lightColor * NdotL * intensity;
}

float3 CalculateDirectionalLight(float3 normal, float3 albedo, float metallic, float roughness, float3 viewDir, DirectionalLight light)
{
    float3 lightDir = normalize(-light.direction);
    return CalculateLightContribution(albedo, metallic, roughness, normal, lightDir, viewDir, light.color, light.intensity);
}

float3 CalculatePointLight(float3 worldPos, float3 normal, float3 albedo, float metallic, float roughness, float3 viewDir, PointLight light)
{
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));
    return CalculateLightContribution(albedo, metallic, roughness, normal, lightDir, viewDir, light.color, light.intensity) * attenuation;
}

float3 CalculateSpotLight(float3 worldPos, float3 normal, float3 albedo, float metallic, float roughness, float3 viewDir, SpotLight light)
{
    float3 lightDir = normalize(light.position - worldPos);
    float distance = length(light.position - worldPos);
    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

    float3 spotDirection = normalize(-light.direction);
    float theta = dot(lightDir, spotDirection);
    float intensity = smoothstep(light.outerConeCos, light.innerConeCos, theta);

    return CalculateLightContribution(albedo, metallic, roughness, normal, lightDir, viewDir, light.color, light.intensity) * attenuation * intensity;
}

float3 ReconstructWorldPosition(float2 texCoords, float depth)
{
    float4 clipSpacePos = float4(texCoords * 2.0 - 1.0, depth, 1.0);
    clipSpacePos.y = -clipSpacePos.y;
    float4 viewSpacePos = mul(clipSpacePos, invViewProj);
    viewSpacePos /= viewSpacePos.w;
    float3 worldPos = viewSpacePos.xyz;

    return worldPos;
}

LightingOutput main(PixelInput input)
{
    LightingOutput output;

    float4 albedoMetallic = g_AlbedoMetallic.Sample(sampleWrap, input.texCoords);
    float4 normalRoughness = g_NormalRoughness.Sample(sampleWrap, input.texCoords);
    float depth = g_Depth.Sample(sampleWrap, input.texCoords);

    float3 albedo = albedoMetallic.rgb;
    float metallic = albedoMetallic.a;
    float3 normal = normalize(normalRoughness.xyz * 2.0f - 1.0f);
    float roughness = normalRoughness.w;

    float3 worldPos = ReconstructWorldPosition(input.texCoords, depth);
    float3 viewDir = normalize(cameraPosition - worldPos);

    float3 finalColor = CalculateDirectionalLight(normal, albedo, metallic, roughness, viewDir, directionalLight);
    for (uint i = 0; i < activePointLightsCount; ++i)
    {
        finalColor += CalculatePointLight(worldPos, normal, albedo, metallic, roughness, viewDir, pointLights[i]);
    }

    float exposure = 1.1;
    finalColor *= exposure;

    output.color = float4(finalColor, 1.0f);
    //output.color = float4(worldPos, 1.0f);
    return output;
}