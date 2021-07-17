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
    void                    Update_CollisionManager(const _float& fTimeDelta);
private:
    void                    Player_to_NPC_Collision();
    void                    Throw_to_NPC_Collision();
    void                    Skill_to_NPC_Collision(const _float& fTimeDelta);
public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
    _float time;
    _bool damage{};
};

