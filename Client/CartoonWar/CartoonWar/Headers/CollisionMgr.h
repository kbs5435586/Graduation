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
private: //AABB
    void                    Player_to_NPC_Collision();
    void                    NPC_to_NPC_Collision();
    void                    Enviroment_to_Player_Or_NPC();
    void                    Deffend();
private://Range
    void                    Range();
private: //OBB
    void                    Player_to_NPC_Attack_Collision();
    void                    Player_to_Player_Attack_Collision();
    void                    NPC_to_NPC_Attack_Collision();
    void                    Player_to_Deffend_Attack_Collision();
    void                    NPC_to_Deffend_Attack_Collision();
private:
    void                    AABB();
    void                    OBB();

public:
    static CCollisionMgr*   Create();
private:
    virtual void            Free();
    
};

