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
        float2(-1.0, -1.0), 
        float2(-1.0, 1.0), 
        float2(1.0, -1.0),
        float2(1.0, -1.0), 
        float2(-1.0, 1.0), 
        float2(1.0, 1.0),
    };

    float2 uvs[6] = {
        float2(0.0, 1.0), 
        float2(0.0, 0.0), 
        float2(1.0, 1.0),
        float2(1.0, 1.0), 
        float2(0.0, 0.0), 
        float2(1.0, 0.0),
    };

    output.position = float4(positions[input.vertexID], 0.0, 1.0);
    output.texCoords = uvs[input.vertexID];

    return output;
}