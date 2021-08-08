#pragma once
#include "Base.h"
class CEventMgr :
    public CBase
{
    _DECLARE_SINGLETON(CEventMgr)
private:
    CEventMgr();
    virtual ~CEventMgr() = default;
public:
    HRESULT                     Ready_EventMgr();
public:
    void                        Update_EventMgr();
    void                        Access_Flag_Player();
    void                        Access_Flag_NPC();
public:
    virtual void				Free();
};

