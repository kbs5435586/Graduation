#pragma once
#include "Component.h"
class CObserver :
    public CComponent
{
private:
    CObserver();
    CObserver(const CObserver& rhs);
    virtual ~CObserver() = default;
public:
    HRESULT                 Ready_Observer();
    void                    Update_Observer(DATA_TYPE eType, void* pData);
public:
    INFO                    GetInfo() { return m_tInfo; }
    _vec3                   GetVec3Info() { return m_vec3; }
public:
    static CObserver*       Create();
    virtual CComponent*     Clone_Component(void* pArg);
private:
    virtual void            Free();
private:
    INFO                    m_tInfo = {};
    _vec3                   m_vec3 = {};

};

