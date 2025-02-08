#ifndef _SGE_MATERIAL_H_
#define _SGE_MATERIAL_H_

#include "data/sge_data_structures.h"
#include "rendering/sge_render_context.h"

namespace SGE
{
    class Material
    {
    public:
        void Initialize(const MaterialAssetData& materialAsset, RenderContext* context);
        void Bind(ID3D12GraphicsCommandList* commandList, DescriptorHeap* heap);

    private:
        uint32 m_albedoTextureIndex;
        uint32 m_metallicTextureIndex;
        uint32 m_normalTextureIndex;
        uint32 m_roughnessTextureIndex;
    };
}

#endif // !_SGE_MATERIAL_H_