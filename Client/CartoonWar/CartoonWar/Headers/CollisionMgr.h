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
    void                    Player_to_Player_Collision();
    void                    NPC_to_NPC_Collision();
    void                    Deffend_to_Player();
    void                    Deffend_to_NPC();
    void                    Deffend_to_Deffend();
private://Range
    void                    Throw_to_NPC_Collision();
    void                    Throw_to_Player_Collision();
    void                    Throw_to_Deffend_Collision();
private: //OBB
    void                    Player_to_NPC_Attack_Collision();
    void                    Player_to_Deffend_Attack_Collision();
    void                    NPC_to_Deffend_Attack_Collision();
private: //UI
    void                    Skill_to_NPC_Collision(const _float& fTimeDelta);
    void                    Teleport_to_NPC_Collision(const _float& fTimeDelta);
private:
    void                    Enviroment_to_Player();
    void                    Enviroment_to_NPC();
public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
    _float time;
    _bool damage{};
};

