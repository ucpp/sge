struct VertexInput
{
    uint vertexID : SV_VertexID;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    float2 positions[6] = {
        float2(-1.0, -1.0), float2(-1.0, 1.0), float2(1.0, -1.0),
        float2(1.0, -1.0), float2(-1.0, 1.0), float2(1.0, 1.0),
    };

    output.position = float4(positions[input.vertexID], 0.0, 1.0);
    output.texCoords = float2((positions[input.vertexID].x + 1.0) * 0.5, (1.0 - positions[input.vertexID].y) * 0.5);

    return output;
}
