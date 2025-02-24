#include "transform_buffer.hlsl"
#include "pixel_input.hlsl"

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

PixelInput TransformVertex(VertexInput input)
{
    PixelInput output;

    float4 worldPosition;
    float4x4 boneTransform;

    if (isAnimated)
    {
        boneTransform = boneTransforms[input.boneIndices[0]] * input.boneWeights[0];
        boneTransform += boneTransforms[input.boneIndices[1]] * input.boneWeights[1];
        boneTransform += boneTransforms[input.boneIndices[2]] * input.boneWeights[2];
        boneTransform += boneTransforms[input.boneIndices[3]] * input.boneWeights[3];

        worldPosition = mul(float4(input.position, 1.0f), boneTransform);
    }
    else
    {
        worldPosition = mul(float4(input.position, 1.0f), model);
    }

    output.worldPosition = worldPosition.xyz;
    float4 viewPos = mul(worldPosition, view);
    output.position = mul(viewPos, projection);

    float3x3 normalMatrix = (float3x3)model;
    if (isAnimated)
    {
        normalMatrix = (float3x3)boneTransform;
    }

    output.normal    = normalize(mul(input.normal, normalMatrix));
    output.tangent   = normalize(mul(input.tangent, normalMatrix));
    output.bitangent = normalize(mul(input.bitangent, normalMatrix));

    output.texCoords = input.texCoords;

    return output;
}


