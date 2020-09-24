#pragma once
#include "Base.h"
class CTimer :
    public CBase
{
private:
    CTimer();
    virtual ~CTimer() = default;

public:
    HRESULT         Ready_Timer();
    const _float&   Get_TimeDelta();
private:
    LARGE_INTEGER   m_frameTime;
    LARGE_INTEGER   m_lastTime;
    LARGE_INTEGER   m_fixTime;
    LARGE_INTEGER   m_CPUTick;
private:
    _float          m_fTimeDelta = 0.f;
public:
    static  CTimer* Create();
    virtual void    Free();

};

