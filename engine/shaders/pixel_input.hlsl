struct PixelInput
{
    float4 position      : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal        : NORMAL;
    float2 texCoords     : TEXCOORD1;
    float3 tangent       : TANGENT;
    float3 bitangent     : BITANGENT;
};