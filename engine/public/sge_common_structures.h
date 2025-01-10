#ifndef _SGE_COMMON_STRUCTURES_H_
#define _SGE_COMMON_STRUCTURES_H_

#include "pch.h"

namespace SGE
{
    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 texCoords;
        Vector3 tangent;
        Vector3 bitangent;
    };

    struct TransformBuffer
    {
        Matrix model;
        Matrix view;
        Matrix projection;
    };
}

#endif // !_SGE_COMMON_STRUCTURES_H_