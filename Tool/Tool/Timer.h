#pragma once
#include "Base.h"
class CTimer :
	public CBase
{
public:
	CTimer();
	virtual ~CTimer()=default;
public:
	HRESULT			Ready_Timer();
	const _float&	Get_TimeDelta();
private:
	LARGE_INTEGER			m_FrameTime;
	LARGE_INTEGER			m_LastTime;
	LARGE_INTEGER			m_FixTime;
	LARGE_INTEGER			m_CpuTick;
private:
	_float					m_fTimeDelta = 0.f;
public:
	static CTimer* Create();
	virtual void Free();
};

