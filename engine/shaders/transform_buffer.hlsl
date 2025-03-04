cbuffer TransformBuffer : register(b1)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix boneTransforms[100];
    bool isAnimated;
    float2 tilingUV;
}