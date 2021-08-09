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
private://Player, NPC
    void                    Player_to_NPC_Collision();
    void                    Player_to_NPC_Attack_Collision();
    void                    Player_to_Player_Collision();
    void                    NPC_to_NPC_Collision();
private://Throw
    void                    Throw_to_NPC_Collision();
    void                    Throw_to_Player_Collision();
private://Deffend
    void                    Deffend_to_Player();
    void                    Deffend_to_NPC();
    void                    Deffend_to_Throw();
    void                    Deffend_to_Deffend();
private:
    void                    Enviroment_to_Player();
    void                    Enviroment_to_NPC();
public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
};

