#ifndef _SGE_TEXTURE_H_
#define _SGE_TEXTURE_H_

#include "pch.h"

namespace SGE
{
    class Texture
    {
    public:
        void Initialize(const std::string& texturePath, class Device* device, class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

    private:
        ComPtr<ID3D12Resource> m_resource = nullptr;
        ComPtr<ID3D12Resource> m_resourceUpload = nullptr;
    };
}

#endif // !_SGE_TEXTURE_H_
