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

    float4 localPosition = float4(input.position, 1.0f);

    if (isAnimated)
    {
        float4x4 boneTransform =
            boneTransforms[input.boneIndices[0]] * input.boneWeights[0] +
            boneTransforms[input.boneIndices[1]] * input.boneWeights[1] +
            boneTransforms[input.boneIndices[2]] * input.boneWeights[2] +
            boneTransforms[input.boneIndices[3]] * input.boneWeights[3];

        localPosition = mul(localPosition, boneTransform);
    }

    float4 worldPosition = mul(localPosition, model);
    float4 viewPos = mul(worldPosition, lightView);
    output.position = mul(viewPos, lightProj);

    return output;
}
