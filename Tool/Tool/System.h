#pragma once
#include "Base.h"
class CTimer_Mgr;
class CFrame_Mgr;
class CSystem :
	public CBase
{
	_DECLARE_SINGLETON(CSystem)
public:
	CSystem();
	virtual ~CSystem()=default;
public:
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
public: 
	HRESULT Add_Frame(const _tchar* pFrameTag, const _float& fCallCnt);
	_bool Permit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
private:
	CTimer_Mgr*			m_pTimer_Mgr = nullptr;
	CFrame_Mgr*			m_pFrame_Mgr = nullptr;
public:
	virtual void Free();
};

