#pragma once
#include "Base.h"
class CUAV :
    public CBase
{
private:
    CUAV();
    virtual ~CUAV() = default;
public:
    HRESULT                                 Ready_UAV(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                                        , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                                        , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr = _vec4());
public:
    static  CUAV*                           Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                                , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                                , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr = _vec4());
public:
    D3D12_RESOURCE_STATES                   GetState() { return m_eState; }
    void                                    SetState(D3D12_RESOURCE_STATES eState) { m_eState = eState; }
public:
    void                                    Dispatch(_uint x, _uint y, _uint z);
public:
    ComPtr<ID3D12DescriptorHeap>            GetSRV() { return m_pSRV; }
    ComPtr<ID3D12DescriptorHeap>            GetUAV() { return m_pUAV; }
    ComPtr<ID3D12Resource>                  GetTexture() { return m_pTexture; }
private:
    D3D12_RESOURCE_DESC                     m_tDesc;
    D3D12_RESOURCE_STATES                   m_eState = {};
private:
    ComPtr<ID3D12Resource>		            m_pTexture;
    ComPtr<ID3D12DescriptorHeap>            m_pSRV;
    ComPtr<ID3D12DescriptorHeap>            m_pRTV;
    ComPtr<ID3D12DescriptorHeap>            m_pDSV;
    ComPtr<ID3D12DescriptorHeap>            m_pUAV;
private:
    virtual void                            Free();
};

    