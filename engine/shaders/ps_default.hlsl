cbuffer TransformBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
}

struct DirectionalLight
{
    float3 direction;
    float3 color;
    float intensity;
};

struct PointLight
{
    float3 position;
    float3 color;
    float intensity;
};

cbuffer LightBuffer : register(b1)
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    float3 cameraPosition;
};

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD;
};

float3 PhongLighting(float3 normal)
{
    float3 lightDir = normalize(directionalLight.direction);
    float diff = max(dot(normal, lightDir), 0.0f);

    return diff * directionalLight.color * directionalLight.intensity;
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 lighting = PhongLighting(normal);
    float4 diffuseColor = diffuseMap.Sample(sampleWrap, input.texCoords);

    return float4(lighting, 1.0f) * diffuseColor;
}