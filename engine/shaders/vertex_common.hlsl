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

    float4 modelPos = mul(model, float4(input.position, 1.0f));
    output.worldPosition = modelPos.xyz;

    float4 viewPos = mul(view, modelPos);

    output.position = mul(projection, viewPos);

    output.normal    = normalize(mul((float3x3)model, input.normal));
    output.tangent   = normalize(mul((float3x3)model, input.tangent));
    output.bitangent = normalize(mul((float3x3)model, input.bitangent));

    output.texCoords = input.texCoords;

    return output;
}
