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
    //void                    Update_CollisionManager(const _float& fTimeDelta);
private: //AABB
    void                    Player_to_NPC_Collision();
    void                    Player_to_Player();
    void                    NPC_to_NPC_Collision();
    void                    Enviroment_to_Player_Or_NPC();
    void                    Deffend();
    void                    Enviroment_to_Animals();
    void                    Animals_to_Animals();
    void                    Player_Or_NPC_Animals();
    void                    Monster_Player_OR_NPC();
private://Range
    void                    Range();
private: //OBB
    void                    Player_to_NPC_Attack_Collision();
    void                    Player_to_Player_Attack_Collision();
    void                    NPC_to_NPC_Attack_Collision();
    void                    Player_to_Deffend_Attack_Collision();
    void                    NPC_to_Deffend_Attack_Collision();
    void                    Player_to_Animals_AttackCollision();
    void                    Player_to_Monster_AttackCollision();
private:
    void                    AABB();
    void                    OBB();

public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
    _float time;
    _bool damage{};
    
};

