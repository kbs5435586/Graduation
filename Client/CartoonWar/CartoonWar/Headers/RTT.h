#pragma once
#include "Base.h"
#include "DirectXTex.h"
#include "DirectXTex.inl"
class CRTT :
    public CBase
{
private:
    CRTT();
    virtual ~CRTT() = default;
public:
    HRESULT                                     Ready_RTT(const _tchar* pTag, ComPtr<ID3D12Resource> pResource);
    void                                        Set_RenderTarget(ID3D12DescriptorHeap* pDsv);
private:
    HRESULT                                     Ready_Component();
public:
    static CRTT*                                Create(const _tchar* pTag, ComPtr<ID3D12Resource> pResource);
private:
    virtual void                                Free();
private:
    ComPtr<ID3D12DescriptorHeap>				m_pRTV = nullptr;
    ComPtr<ID3D12DescriptorHeap>				m_pSRV = nullptr;
    ComPtr<ID3D12DescriptorHeap>				m_pDSV = nullptr;
    ComPtr<ID3D12Resource>                      m_pTexture = nullptr;
    ScratchImage			 	                m_Image;
private:
    D3D12_RESOURCE_DESC			                m_tDesc;
private:
    _float                                      m_fClearColor[4] = {0.f,0.f,1.f,1.f};
    _tchar                                      m_pRtt_Tag[512] = {};
};

