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
private:
    void                    Player_to_NPC_Collision();
    void                    Throw_to_NPC_Collision();
public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
};
