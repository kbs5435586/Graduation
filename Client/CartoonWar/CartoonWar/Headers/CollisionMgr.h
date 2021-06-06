#pragma once
#include "Base.h"
class CCollisionMgr :
    public CBase
{
    _DECLARE_SINGLETON(CCollisionMgr)
private:
    CCollisionMgr();
    virtual ~CCollisionMgr() = default;
public:
    HRESULT                 Ready_CollsionManager();
    void                    Update_CollisionManager();
public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
};

