#include "scene_data.hlsl"
#include "transform_buffer.hlsl"

struct VertexInput
{
    float3 position      : POSITION;
    float3 normal        : NORMAL;
    float2 texCoords     : TEXCOORD;
    float3 tangent       : TANGENT;
    float3 bitangent     : BITANGENT;
    float4 boneWeights   : BONE_WEIGHTS;
    int4   boneIndices   : BONE_INDICES;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VertexInput input)
{
    VSOutput output;
    float4 modelPos = mul(float4(input.position, 1.0f), model);
    float4 viewPos = mul(modelPos, lightView);
    output.position = mul(viewPos, lightProj);

    return output;
}
