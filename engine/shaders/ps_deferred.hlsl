cbuffer TransformBuffer : register(b1)
{
    matrix model;
    matrix view;
    matrix projection;
}

struct DirectionalLight
{
    float3 direction;
    float intensity;
    float3 color;
};

struct PointLight
{
    float3 position;
    float intensity;
    float3 color;
};

cbuffer SceneData : register(b0)
{
    DirectionalLight directionalLight;
    PointLight pointLight;
    float3 cameraPosition;
    float fogStrength;
    float3 fogColor;
    float fogStart;
    float fogEnd;
    float fogDensity;
};

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
SamplerState sampleWrap : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

float4 main(PixelInput input) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}