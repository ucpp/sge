#ifndef _SGE_MESH_H_
#define _SGE_MESH_H_

#include <vector>
#include "sge_types.h"
#include "sge_data_structures.h"

namespace SGE
{
    struct MeshResourceInfo
    {
        uint32 meshIndexCount;
        uint32 vertexCountOffset;
        uint32 indexCountOffset;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
            : m_vertices(vertices)
            , m_indices(indices) {}

        void UpdateInfo(const MeshResourceInfo& info) { m_info = info; }

        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        const std::vector<uint32>& GetIndices() const { return m_indices; }
        const MeshResourceInfo& GetInfo() const { return m_info; }

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32> m_indices;
        MeshResourceInfo m_info;
    };
}

#endif // !_SGE_MESH_H_