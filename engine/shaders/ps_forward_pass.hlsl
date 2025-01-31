#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"
#include "scene_data.hlsl"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D roughnessMap : register(t3);

SamplerState sampleWrap : register(s0);

float LinearFog(float distance, float fogStart, float fogEnd, float fogStrength)
{
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    fogFactor *= fogStrength;
    return fogFactor;
}

float ExponentialFog(float distance, float fogDensity, float fogStrength)
{
    float fogFactor = 1.0f - exp(-distance * fogDensity);
    fogFactor *= fogStrength;
    return fogFactor;
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 normalMapValue = normalMap.Sample(sampleWrap, input.texCoords).xyz * 2.0f - 1.0f;

    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = normalize(mul(normalMapValue, TBN));

    float3 lightDir = normalize(-directionalLight.direction);

    float3 viewDir = normalize(cameraPosition - input.worldPosition);

    float diff = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diff * directionalLight.color * directionalLight.intensity;

    float3 reflectionDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), 32.0f); // Shininess exponent (adjust as needed)

    float specularColor = specularMap.Sample(sampleWrap, input.texCoords).r;
    float3 specular = spec * specularColor * directionalLight.color * directionalLight.intensity;

    float4 diffuseColor = diffuseMap.Sample(sampleWrap, input.texCoords);
    float4 finalColor = float4(diffuse + specular, 1.0f) * diffuseColor;

    float distance = length(cameraPosition - input.worldPosition);

    // float fogFactor = LinearFog(distance, fogStart, fogEnd, fogStrength);
    float fogFactor = ExponentialFog(distance, fogDensity, fogStrength);

    float3 foggedColor = lerp(finalColor.rgb, fogColor, fogFactor);

    return float4(foggedColor, finalColor.a);
}