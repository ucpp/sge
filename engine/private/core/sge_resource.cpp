#include "core/sge_resource.h"

namespace SGE
{
    Resource::Resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState)
    : m_resource(resource)
    , m_currentState(initialState) {}

    ID3D12Resource* Resource::Get() const
    {
        return m_resource.Get();
    }

    D3D12_RESOURCE_STATES Resource::GetCurrentState() const
    {
        return m_currentState;
    }

    void Resource::TransitionState(D3D12_RESOURCE_STATES newState, std::vector<CD3DX12_RESOURCE_BARRIER>& barriers)
    {
        if (m_currentState != newState)
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), m_currentState, newState);
            barriers.push_back(barrier);
            m_currentState = newState;
        }
    }

    void Resource::TransitionState(D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* commandList)
    {
        if (m_currentState != newState)
        {
            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), m_currentState, newState));
            m_currentState = newState;
        }
    }
}