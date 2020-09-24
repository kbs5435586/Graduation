#pragma once
#include "Base.h"
class CTimerManager;
class CFrameManager;
class CSystem :
    public CBase
{
    _DECLARE_SINGLETON(CSystem);
private:
    CSystem();
	virtual ~CSystem() = default;
public: 
	HRESULT				Add_Timer(const _tchar* pTimerTag);
	_float				Get_TimeDelta(const _tchar* pTimerTag);
public: 
	HRESULT				Add_Frame(const _tchar* pFrameTag, const _float& fCallCnt);
	_bool				Permit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
private:
	CTimerManager*		m_pTimer_Manager = nullptr;
	CFrameManager*		m_pFrame_Manager = nullptr;
protected:
	virtual void		Free();
};

