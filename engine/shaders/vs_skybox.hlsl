#include "scene_data.hlsl"

struct VertexInput
{
    uint vertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    float3 positions[36] = {
        float3(-1, -1,  1), float3( 1, -1,  1), float3( 1,  1,  1),
        float3(-1, -1,  1), float3( 1,  1,  1), float3(-1,  1,  1),

        float3( 1, -1, -1), float3(-1, -1, -1), float3(-1,  1, -1),
        float3( 1, -1, -1), float3(-1,  1, -1), float3( 1,  1, -1),

        float3(-1, -1, -1), float3(-1, -1,  1), float3(-1,  1,  1),
        float3(-1, -1, -1), float3(-1,  1,  1), float3(-1,  1, -1),

        float3( 1, -1,  1), float3( 1, -1, -1), float3( 1,  1, -1),
        float3( 1, -1,  1), float3( 1,  1, -1), float3( 1,  1,  1),

        float3(-1,  1,  1), float3( 1,  1,  1), float3( 1,  1, -1),
        float3(-1,  1,  1), float3( 1,  1, -1), float3(-1,  1, -1),

        float3(-1, -1, -1), float3( 1, -1, -1), float3( 1, -1,  1),
        float3(-1, -1, -1), float3( 1, -1,  1), float3(-1, -1,  1),
    };

    float3 pos = positions[input.vertexID];

    output.position = mul(float4(pos, 1.0), viewProjSky).xyww;
    output.texCoord = pos;

    return output;
}