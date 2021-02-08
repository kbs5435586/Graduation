#pragma once
#include "Base.h"
class CComponent :
    public CBase
{
protected:
    CComponent();
    CComponent(const CComponent& rhs);
    virtual ~CComponent()=default;
public:
    HRESULT         Ready_Component();
protected:
    _bool           m_IsClone = false;
public:
    virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
protected:
    virtual void Free();
};

