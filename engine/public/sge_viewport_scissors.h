#ifndef _SGE_VIEWPORT_SCISSORS_H_
#define _SGE_VIEWPORT_SCISSORS_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>

namespace SGE
{
    class ViewportScissors final
    {
    public:
        ViewportScissors(UINT width = 800, UINT height = 600, float minDepth = D3D12_MIN_DEPTH, float maxDepth = D3D12_MAX_DEPTH);
        void Set(UINT width, UINT height);
        void Bind(ID3D12GraphicsCommandList* commandList);

        CD3DX12_VIEWPORT GetViewport() const { return m_viewport; }
        CD3DX12_RECT GetScissorRect() const { return m_scissorRect; }
        UINT GetWidth() const { return static_cast<UINT>(m_viewport.Width); }
        UINT GetHeight() const { return static_cast<UINT>(m_viewport.Height); }

    private:
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
        float m_minDepth;
        float m_maxDepth;
    };
}

#endif // !_SGE_VIEWPORT_SCISSORS_H_