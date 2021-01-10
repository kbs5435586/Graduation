#pragma once
#include "Base.h"
class CTimer;
class CTimerManager :
    public CBase
{
    _DECLARE_SINGLETON(CTimerManager)
private:
    CTimerManager();
	virtual ~CTimerManager() = default;
public:
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
private:
	map<const _tchar*, CTimer*>			m_mapTimers;
	typedef map<const _tchar*, CTimer*>	MAPTIMERS;
private:
	CTimer* Find_Timer(const _tchar* pTimerTag);
public:
	virtual void Free();
};

