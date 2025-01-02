cbuffer TransformBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float lodLevel;
}

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORD;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    float4 modelPos = mul(float4(input.position, 1.0f), model);
    float4 viewPos = mul(modelPos, view);
    output.position = mul(viewPos, projection);

    output.normal = input.normal;
    output.texCoords = input.texCoords;

    return output;
}
