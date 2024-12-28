#ifndef _SGE_MODEL_H_
#define _SGE_MODEL_H_

#include <vector>
#include "sge_mesh.h"

namespace SGE
{
    class Model
    {
    public:
        void AddMesh(const Mesh& mesh) { m_meshes.push_back(mesh); }
        const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

    private:
        std::vector<Mesh> m_meshes;
    };
}

#endif // !_SGE_MODEL_H_