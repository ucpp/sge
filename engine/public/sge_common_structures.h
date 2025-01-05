#ifndef _SGE_COMMON_STRUCTURES_H_
#define _SGE_COMMON_STRUCTURES_H_

#include <DirectXMath.h>
using namespace DirectX;

namespace SGE
{
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texCoords;
        XMFLOAT3 tangent;
        XMFLOAT3 bitangent;
    };

    struct TransformBuffer
    {
        XMMATRIX model;
        XMMATRIX view;
        XMMATRIX projection;
    };
}

#endif // !_SGE_COMMON_STRUCTURES_H_