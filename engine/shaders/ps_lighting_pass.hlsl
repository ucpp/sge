#include "scene_data.hlsl"

Texture2D<float4> g_AlbedoMetallic : register(t0);  // Albedo + Metallic
Texture2D<float4> g_NormalRoughness : register(t1); // Normal + Roughness
Texture2D<float> g_Depth : register(t2);            // Depth

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

float3 calculateDirectionalLightOld(float3 normal, float3 albedo, DirectionalLight light)
{
    float3 lightDir = normalize(-light.direction); 
    float diff = max(dot(normal, lightDir), 0.0);
    return albedo * light.color * diff * light.intensity;
}

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
    denom = 3.14159265359 * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 calculateDirectionalLight(float3 normal, float3 albedo, float metallic, float roughness, float3 worldPos, float3 viewDir, DirectionalLight light)
{
    float3 lightDir = normalize(-light.direction);
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
    return (kD * albedo / 3.14159265359 + specular) * light.color * NdotL * light.intensity;
}

float3 calculatePointLight(float3 worldPos, float3 normal, float3 albedo, PointLight light)
{
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    float diff = max(dot(normal, lightDir), 0.0);

    return albedo * light.color * diff * light.intensity * attenuation;
}

float3 calculateSpotLight(float3 worldPos, float3 normal, float3 albedo, SpotLight light)
{
    float3 lightDir = normalize(light.position - worldPos);
    float distance = length(light.position - worldPos);

    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

    float3 spotDirection = normalize(-light.direction);
    float theta = dot(lightDir, spotDirection);
    float epsilon = light.innerConeCos - light.outerConeCos;
    float intensity = smoothstep(light.outerConeCos, light.innerConeCos, theta);

    float diff = max(dot(normal, lightDir), 0.0);

    return albedo * light.color * diff * light.intensity * attenuation * intensity;
}

float3 ReconstructWorldPos(float2 uv, float depth, float4x4 invViewProj, float zNear, float zFar)
{
    float zLinear = zNear * zFar / (zFar - depth * (zFar - zNear));
    float4 clipPos = float4(uv * 2.0 - 1.0, zLinear, 1.0);
    clipPos.y = -clipPos.y;
    float4 worldPos = mul(clipPos, invViewProj);

    worldPos /= worldPos.w;

    return worldPos.xyz;
}

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float3 ReinhardToneMapping(float3 color)
{
    return color / (color + 1.0);
}

float3 SimpleToneMapping(float3 color, float exposure)
{
    color *= exposure;
    return pow(color, 1.0 / 2.2);
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

    float3 worldPos = ReconstructWorldPos(input.texCoords, depth, invViewProj, zNear, zFar);
    float3 viewDir = normalize(cameraPosition - worldPos);

    float3 finalColor = calculateDirectionalLight(normal, albedo, metallic, roughness, worldPos, viewDir, directionalLight);
    for (uint i = 0; i < activePointLightsCount; ++i)
    {
        finalColor += calculatePointLight(worldPos, normal, albedo, pointLights[i]);
    }

    float exposure = 1.5;
    finalColor *= exposure;

    finalColor = ACESFilm(finalColor);
    finalColor = pow(finalColor, 1.0 / 2.2);

    output.color = float4(finalColor, 1.0f);
    return output;
}
