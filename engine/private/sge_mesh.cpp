#include "sge_mesh.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_texture_manager.h"

namespace SGE
{
    void Mesh::Initialize(Device* device, DescriptorHeap* descriptorHeap, uint32 descriptorIndex) const
    {
        TextureManager::GetTexture(m_material.diffuseTexturePath, device, descriptorHeap, descriptorIndex);
    }
}