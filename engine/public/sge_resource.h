#ifndef _SGE_RESOURCE_H_
#define _SGE_RESOURCE_H_

#include "pch.h"

namespace SGE
{
    class Resource
    {
    public:
        Resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);
        
        ID3D12Resource* Get() const;
        D3D12_RESOURCE_STATES GetCurrentState() const;

        void TransitionState(D3D12_RESOURCE_STATES newState, std::vector<CD3DX12_RESOURCE_BARRIER>& barriers);
        void TransitionState(D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* commandList);
        
    private:
        ComPtr<ID3D12Resource> m_resource;
        D3D12_RESOURCE_STATES m_currentState;
    };
}

#endif // !_SGE_RESOURCE_H_