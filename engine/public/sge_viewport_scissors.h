#ifndef _SGE_VIEWPORT_SCISSORS_H_
#define _SGE_VIEWPORT_SCISSORS_H_

#include "pch.h"

namespace SGE
{
    class ViewportScissors final
    {
    public:
        ViewportScissors(uint32 width, uint32 height, float minDepth = D3D12_MIN_DEPTH, float maxDepth = D3D12_MAX_DEPTH);
        void Set(uint32 width, uint32 height);
        void Bind(ID3D12GraphicsCommandList* commandList);

        CD3DX12_VIEWPORT GetViewport() const { return m_viewport; }
        CD3DX12_RECT GetScissorRect() const { return m_scissorRect; }
        uint32 GetWidth() const { return static_cast<uint32>(m_viewport.Width); }
        uint32 GetHeight() const { return static_cast<uint32>(m_viewport.Height); }

    private:
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT     m_scissorRect;
        float m_minDepth;
        float m_maxDepth;
    };
}

#endif // !_SGE_VIEWPORT_SCISSORS_H_