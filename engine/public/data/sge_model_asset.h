#ifndef _SGE_MODEL_ASSET_H_
#define _SGE_MODEL_ASSET_H_

#include "pch.h"
#include "data/sge_mesh.h"

namespace SGE
{
    class ModelAsset
    {
    public:
        void Initialize(std::vector<Mesh>& meshes);
        const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        const std::vector<uint32>& GetIndices() const { return m_indices; }
    
    private:
        std::vector<Mesh> m_meshes;
        std::vector<Vertex> m_vertices;
        std::vector<uint32> m_indices;
    };
}

#endif // !_SGE_MODEL_ASSET_H_