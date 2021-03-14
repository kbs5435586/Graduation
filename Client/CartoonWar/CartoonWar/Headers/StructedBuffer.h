#pragma once
#include "Base.h"
class CStructedBuffer :
    public CBase
{
private:
    CStructedBuffer();
    virtual ~CStructedBuffer() = default;
public:
    HRESULT                         Ready_StructedBuffer(_uint iElementSize, _uint iElementCnt, void* pArg);
    void                            Update_Data(TEXTURE_REGISTER eRegister);
    void                            Update_Data_CS(TEXTURE_REGISTER eRegister);
    void                            Update_RWData(UAV_REGISTER eRegister);
public:
    ComPtr<ID3D12DescriptorHeap>    GetSRV() { return m_pSRV; }
    ComPtr<ID3D12DescriptorHeap>    GetUAV() { return m_pUAV; }
    ComPtr<ID3D12Resource>          GetBuffer() { return m_pBuffer; }
public:
    D3D12_RESOURCE_STATES           GetState() { return m_eState; }
    void                            SetState(D3D12_RESOURCE_STATES eState) { m_eState = eState; }
public:
    static CStructedBuffer*         Create(_uint iElementSize, _uint iElementCnt, void* pArg = nullptr);
private:
    virtual void                    Free();
private:
    ComPtr<ID3D12Resource>          m_pBuffer = nullptr;
    ComPtr<ID3D12DescriptorHeap>    m_pSRV = nullptr;
    ComPtr<ID3D12DescriptorHeap>    m_pUAV = nullptr;
private:
    _uint                           m_iElementSize=0;
    _uint                           m_iElementCnt=0;
    D3D12_RESOURCE_STATES           m_eState = D3D12_RESOURCE_STATE_COMMON;
};

