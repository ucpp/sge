cbuffer ModelBuffer : register(b0)
{
    matrix model;
}

cbuffer ViewBuffer : register(b1)
{
    matrix view;
}

cbuffer ProjectionBuffer : register(b2)
{
    matrix projection;
}

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    float4 modelPos = mul(float4(input.position, 1.0f), model);
    float4 viewPos = mul(modelPos, view);
    output.position = mul(viewPos, projection);

    output.color = input.color;
    return output;
}
