#pragma once
#include "Base.h"
class CBuffer_RectTex;
class CShader;
class CRTT :
    public CBase
{
private:
    CRTT();
    virtual ~CRTT() = default;
public:
    HRESULT                                     Ready_RTT(_uint iTextureWidth, _uint iTextureHeight);
    void                                        Set_RenderTarget(ID3D12DescriptorHeap* pDsv);
private:
    HRESULT                                     Ready_Component();
    HRESULT                                     Crate_InputLayOut();
public:
    static CRTT*                                Create(_uint iTextureWidth, _uint iTextureHeight);
private:
    virtual void                                Free();
private:
    ComPtr<ID3D12DescriptorHeap>				m_pRTV = nullptr;
    ComPtr<ID3D12DescriptorHeap>				m_pSRV = nullptr;
    ComPtr<ID3D12Resource>                      m_pTexture = nullptr;
    _uint                                       m_iTextureWidth = 0;
    _uint                                       m_iTextureHeight = 0;
private:
    CBuffer_RectTex*                            m_pBufferCom = nullptr;
    CShader*                                    m_pShaderCom = nullptr;
private:
    _float                                      m_fClearColor[4] = {0.f,0.f,1.f,1.f};
};

