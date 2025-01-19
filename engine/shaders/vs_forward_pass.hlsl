#include "vertex_common.hlsl"

PixelInput main(VertexInput input)
{
    return TransformVertex(input);
}
