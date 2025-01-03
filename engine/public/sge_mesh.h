#ifndef _SGE_MESH_H_
#define _SGE_MESH_H_

#include <vector>
#include "sge_types.h"
#include "sge_common_structures.h"
#include "sge_material_data.h"

namespace SGE
{
    struct MeshResourceInfo
    {
        uint32 diffuseTextureIndex;
        uint32 normalTextureIndex;
        uint32 specularTextureIndex;
        uint32 meshIndexCount;
        uint32 vertexCountOffset;
        uint32 indexCountOffset;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices, const MaterialData& material)
            : m_vertices(vertices)
            , m_indices(indices)
            , m_material(material) {}

        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        const std::vector<uint32>& GetIndices() const { return m_indices; }
        const MaterialData& GetMaterial() const { return m_material; }

        void Initialize(class Device* device, class DescriptorHeap* descriptorHeap);

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32> m_indices;
        MaterialData m_material;
    };
}

#endif // !_SGE_MESH_H_