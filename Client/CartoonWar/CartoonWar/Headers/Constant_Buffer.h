#pragma once
#include "Base.h"
class CConstant_Buffer :
    public CBase
{
private:
    CConstant_Buffer();
    virtual ~CConstant_Buffer() = default;
public:
    HRESULT                             Create_ConstantBuffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType);
public:
    static  CConstant_Buffer*           Create(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType);
private:
    ComPtr<ID3D12DescriptorHeap>        m_pCBV = nullptr ;
    ComPtr<ID3D12Resource>              m_pBuffer = nullptr ;
    D3D12_CPU_DESCRIPTOR_HANDLE		    m_hCBV;
    D3D12_DESCRIPTOR_HEAP_DESC		    m_tHeapDesc;
private:
    _uint                               m_iBufferSize = 0;
    _uint                               m_iMaxCnt = 0;
    _uint                               m_iCurrCnt = 0;
    BYTE*                               m_pData= nullptr;
private:
    CONST_REGISTER                      m_eType = CONST_REGISTER::END;
public:
    _uint                               SetData(void* pArg);
    void                                ResetCount();
public:
    ComPtr<ID3D12DescriptorHeap>        GetCBV() { return m_pCBV; }
    D3D12_CPU_DESCRIPTOR_HANDLE         GetHandle() { return m_hCBV; }
    CONST_REGISTER                      GetConstantType() { return m_eType; }

private:
    virtual void                        Free();
};

