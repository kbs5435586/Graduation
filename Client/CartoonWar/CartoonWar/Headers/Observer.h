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
    void                    ReUpdate_Observer(DATA_TYPE eType);
public:
    INFO                    GetInfo() { return m_tInfo; }
    _vec3                   GetVec3Info() { return m_vec3; }
    _bool                   GetIntInfo() { return m_int; }
    _int                    GetWhichInfo() { return m_which; }
    _bool                   GetBoolInfo() { return m_bool; }
    _bool                   GetTapInfo() { return m_tap; }
    void*                   GetNPC(int num);
public:
    static CObserver*       Create();
    virtual CComponent*     Clone_Component(void* pArg);
private:
    virtual void            Free();
private:
    INFO                    m_tInfo = {};
    _vec3                   m_vec3 = {};
    _bool                    m_int = {};
    _int                    m_which = {};
    _bool                   m_bool = false;
    _bool                   m_tap = false;
    _bool                   m_skill{};
    list<void*>*            m_lstData;
};

