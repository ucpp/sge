#include "data/sge_material.h"

#include "data/sge_texture_manager.h"

namespace SGE
{
    void Material::Initialize(const MaterialAssetData& materialAsset, RenderContext* context)
    {
        m_albedoTextureIndex = TextureManager::GetTextureIndex(materialAsset.albedoTexturePath, TextureType::Albedo, context->GetDevice(), context->GetCbvSrvUavHeap());
        m_normalTextureIndex = TextureManager::GetTextureIndex(materialAsset.normalTexturePath, TextureType::Normal, context->GetDevice(), context->GetCbvSrvUavHeap());
        m_metallicTextureIndex = TextureManager::GetTextureIndex(materialAsset.metallicTexturePath, TextureType::Metallic, context->GetDevice(), context->GetCbvSrvUavHeap());
        m_roughnessTextureIndex = TextureManager::GetTextureIndex(materialAsset.roughnessTexturePath, TextureType::Roughness, context->GetDevice(), context->GetCbvSrvUavHeap());
    }
    
    void Material::Bind(ID3D12GraphicsCommandList* commandList, DescriptorHeap* heap)
    {
        commandList->SetGraphicsRootDescriptorTable(2, heap->GetGPUHandle(m_albedoTextureIndex));
        commandList->SetGraphicsRootDescriptorTable(3, heap->GetGPUHandle(m_normalTextureIndex));
        commandList->SetGraphicsRootDescriptorTable(4, heap->GetGPUHandle(m_metallicTextureIndex));
        commandList->SetGraphicsRootDescriptorTable(5, heap->GetGPUHandle(m_roughnessTextureIndex));
    }
}