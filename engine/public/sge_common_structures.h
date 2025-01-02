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
    };

    struct TransformBuffer
    {
        XMMATRIX model;
        XMMATRIX view;
        XMMATRIX projection;
        // ONLY FOR TEST!!!
        float lodLevel;
    };
}

#endif // !_SGE_COMMON_STRUCTURES_H_