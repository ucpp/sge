#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D<float> g_ShadowMap : register(t4);

SamplerState sampleWrap : register(s0);
SamplerState sampleClamp : register(s1);

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
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float ggx1 = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx2 = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}

float3 CalculateLightContribution(float3 albedo, float metallic, float roughness, float3 normal, float3 lightDir, float3 viewDir, float3 lightColor, float intensity)
{
    float3 halfDir = normalize(viewDir + lightDir);
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    float NDF = DistributionGGX(normal, halfDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    
    float3 specular = (NDF * G * F) / max(4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0), 0.001);
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);

    float3 diffuse = kD * albedo / PI;
    float NdotL = max(dot(normal, lightDir), 0.0);
    
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
    float radiusAttenuation = max(0.0f, (light.radius - distance) / light.radius);
    attenuation *= radiusAttenuation;
    
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
            float shadowMapDepth = shadowMap.Sample(samplerState, shadowCoord.xy + offset);
            shadow += shadowCoord.z - bias > shadowMapDepth ? 1.0 : 0.0;
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

    float bias = 0.02;
    return CalculateShadowPCF(shadowMap, samplerState, shadowCoord.xyz, bias);
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 albedo = diffuseMap.Sample(sampleWrap, input.texCoords).rgb;
    float3 normalMapValue = normalMap.Sample(sampleWrap, input.texCoords).xyz * 2.0f - 1.0f;
    float metallic = metallicMap.Sample(sampleWrap, input.texCoords).r;
    float roughness = roughnessMap.Sample(sampleWrap, input.texCoords).r;
    
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = normalize(mul(normalMapValue, TBN));
    float3 viewDir = normalize(cameraPosition - input.worldPosition);

    float shadow = CalculateShadow(g_ShadowMap, sampleClamp, input.worldPosition);
    float shadowFactor = 1.0 - shadow;
    
    float3 finalColor = CalculateDirectionalLight(normal, albedo, metallic, roughness, viewDir, directionalLight) * shadowFactor;
    for (uint i = 0; i < activePointLightsCount; ++i)
    {
        finalColor += CalculatePointLight(input.worldPosition, normal, albedo, metallic, roughness, viewDir, pointLights[i]);
    }
    for (uint i = 0; i < activeSpotLightsCount; ++i)
    {
        finalColor += CalculateSpotLight(input.worldPosition, normal, albedo, metallic, roughness, viewDir, spotLights[i]);
    }
    
    return float4(finalColor, 1.0);
}
