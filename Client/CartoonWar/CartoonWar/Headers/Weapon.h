#pragma once
#include "GameObject.h"
class CStructedBuffer;
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
protected:
    CStructedBuffer*                        m_pStructedBuffer = nullptr;
    _bool                                   m_IsPicked = false;
    _uint                                   m_iBoneIdx = 0;
public:
    _bool&                                  GetIsPicked() { return m_IsPicked; }
    CStructedBuffer*&                       GetStructedBuffer() { return m_pStructedBuffer; }
    _uint&                                  GetBoneIdx(){return m_iBoneIdx;}
};

