#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"

struct VertexInput
{
    float3 position  : POSITION;
    float3 normal    : NORMAL;
    float2 texCoords : TEXCOORD;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

PixelInput TransformVertex(VertexInput input)
{
    PixelInput output;

    float4 modelPos = mul(float4(input.position, 1.0f), model);
    output.worldPosition = modelPos.xyz;

    float4 viewPos = mul(modelPos, view);
    output.position = mul(viewPos, projection);

    output.normal    = normalize(mul(input.normal, (float3x3)model));
    output.tangent   = normalize(mul(input.tangent, (float3x3)model));
    output.bitangent = normalize(mul(input.bitangent, (float3x3)model));

    output.texCoords = input.texCoords;

    return output;
}


