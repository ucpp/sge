#include "sge_viewport_scissors.h"

#include "sge_logger.h"

namespace SGE
{
    ViewportScissors::ViewportScissors(uint32 width, uint32 height, float minDepth, float maxDepth)
    : m_minDepth(minDepth)
    , m_maxDepth(maxDepth)
    {
        Set(width, height);
    }
    
    void ViewportScissors::Set(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0)
        {
            LOG_ERROR("Invalid dimensions for ViewportScissors.");
            return;
        }

        m_viewport.TopLeftX = 0.0f;
        m_viewport.TopLeftY = 0.0f;
        m_viewport.Width = static_cast<float>(width);
        m_viewport.Height = static_cast<float>(height);
        m_viewport.MinDepth = m_minDepth;
        m_viewport.MaxDepth = m_maxDepth;

        m_scissorRect.left = 0;
        m_scissorRect.top = 0;
        m_scissorRect.right = static_cast<LONG>(width);
        m_scissorRect.bottom = static_cast<LONG>(height);
    }
    
    void ViewportScissors::Bind(ID3D12GraphicsCommandList* commandList)
    {
        if (!commandList)
        {
            LOG_ERROR("ViewportScissors::Bind called with nullptr for commandList.");
            return;
        }

        commandList->RSSetViewports(1, &m_viewport);
        commandList->RSSetScissorRects(1, &m_scissorRect);
    }
}