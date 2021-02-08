#pragma once
#include "Base.h"
class CKeyManager :
    public CBase
{
    _DECLARE_SINGLETON(CKeyManager)
private:
    CKeyManager();
    virtual ~CKeyManager() = default;
public:
    HRESULT                         Ready_KeyManager();
    void                            Key_Update();
    _bool                           Key_Up(DWORD dwKey);
    _bool                           Key_Down(DWORD dwKey);
    _bool                           Key_Pressing(DWORD dwKey);
    _bool                           Key_Combine(DWORD dwFirstKey, DWORD dwSecondKey);
private:
    DWORD                           m_dwKey = 0;
    DWORD                           m_dwKeyPressed = 0;
    DWORD                           m_dwKeyDown = 0;
public:
    static CKeyManager*             Create();
private:
    virtual void Free(); 
};

