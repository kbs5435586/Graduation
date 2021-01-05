#pragma once
#include "Base.h"
class CTimer;
class CTimer_Mgr :
	public CBase
{
	_DECLARE_SINGLETON(CTimer_Mgr)
public:
	CTimer_Mgr();
	virtual ~CTimer_Mgr()=default;
public:
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	_float		Get_TimeDelta(const _tchar* pTimerTag);
private:
	CTimer*		Find_Timer(const _tchar* pTimerTag);
public:
	virtual void Free();
private:
	map<const _tchar*, CTimer*>			m_mapTImer;

};

