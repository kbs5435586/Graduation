#pragma once
#include "Base.h"

class CRTT :
    public CBase
{
private:
    CRTT();
    virtual ~CRTT() = default;

public:
    HRESULT                         CreateFromResource(const _tchar* pTag, ComPtr<ID3D12Resource> _pTex2D);
    HRESULT                         Create_Texture(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                                    , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                                    , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr);
    HRESULT                         Create_InvenTexture(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                                    , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                                    , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearColor);
    HRESULT                         Create_MapTexture(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                                    , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                                    , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearColor);
public:
    D3D12_RESOURCE_STATES           GetState() { return m_eState; }
    void                            SetState(D3D12_RESOURCE_STATES eState) { m_eState = eState; }
public:
    ComPtr<ID3D12Resource>          GetTex2D()  { return m_pTexture; }
    ComPtr<ID3D12DescriptorHeap>    GetSRV()    { return m_pSRV; }
    ComPtr<ID3D12DescriptorHeap>    GetRTV()    { return m_pRTV; }
    ComPtr<ID3D12DescriptorHeap>    GetDSV()    { return m_pDSV; }


public:
    static CRTT*                    Create(const _tchar* pTag, ComPtr<ID3D12Resource> _pTex2D);
    static CRTT*                    Create(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                            , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                            , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr = _vec4());
    static CRTT*                    CreateInven(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                            , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                            , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr = _vec4());

    static CRTT*                    CreateMap(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
                                            , const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
                                            , D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr = _vec4());


    const _tchar*                   Get_MRT_Tag() { return m_pTag; }
public:
    D3D12_RESOURCE_DESC             Get_Desc() { return m_tDesc; }
public:
private:
    virtual void                    Free();
private:
    D3D12_RESOURCE_DESC             m_tDesc;
    ComPtr<ID3D12Resource>		    m_pTexture;
    ComPtr<ID3D12DescriptorHeap>    m_pSRV;
    ComPtr<ID3D12DescriptorHeap>    m_pRTV;
    ComPtr<ID3D12DescriptorHeap>    m_pDSV;
 
private:
    _tchar                          m_pTag[128] = {};
    D3D12_RESOURCE_STATES           m_eState = {};

};

