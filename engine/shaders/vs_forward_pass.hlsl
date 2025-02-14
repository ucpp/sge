#include "vertex_common.hlsl"
#include "scene_data.hlsl"

PixelInput main(VertexInput input)
{
    return TransformVertex(input);
}
