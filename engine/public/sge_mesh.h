#ifndef _SGE_MESH_H_
#define _SGE_MESH_H_

#include <vector>
#include "sge_common_structures.h"

namespace SGE
{
    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
            : m_vertices(vertices)
            , m_indices(indices) {}

        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        const std::vector<unsigned int>& GetIndices() const { return m_indices; }

    private:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
    };
}

#endif // !_SGE_MESH_H_