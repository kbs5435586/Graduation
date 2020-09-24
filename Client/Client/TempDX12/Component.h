#pragma once
#include "Base.h"
class CComponent :
    public CBase
{
protected:
    CComponent(ID3D12Device* pGraphic_Device);
    CComponent(const CComponent& rhs);
    virtual ~CComponent()=default;
public:
    HRESULT         Ready_Component();
protected:
    ID3D12Device*   m_pGraphic_Device = nullptr;
    _bool           m_IsClone = false;
public:
    virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
protected:
    virtual void Free();
};

