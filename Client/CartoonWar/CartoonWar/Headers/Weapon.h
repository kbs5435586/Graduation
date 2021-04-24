#pragma once
#include "GameObject.h"
class CWeapon :
    public CGameObject
{
protected:
    CWeapon();
    CWeapon(const CWeapon& rhs);
   virtual  ~CWeapon() = default;
public:
    virtual CGameObject*                    Clone_GameObject(void* pArg, const _uint& iIdx = 0) PURE;
private:
    virtual void							Free()PURE;
};

