#include "stdafx.h"
#include "System.h"
#include "Frame_Mgr.h"
#include "Timer_Mgr.h"

_IMPLEMENT_SINGLETON(CSystem)
CSystem::CSystem()
	: m_pTimer_Mgr(CTimer_Mgr::GetInstance())
	, m_pFrame_Mgr(CFrame_Mgr::GetInstance())
{
	m_pTimer_Mgr->AddRef();
	m_pFrame_Mgr->AddRef();
}

HRESULT CSystem::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Mgr)
		return E_FAIL;

	return m_pTimer_Mgr->Add_Timer(pTimerTag);
}

_float CSystem::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Mgr)
		return 0.0f;

	return m_pTimer_Mgr->Get_TimeDelta(pTimerTag);
}

HRESULT CSystem::Add_Frame(const _tchar * pFrameTag, const _float & fCallCnt)
{
	if (nullptr == m_pFrame_Mgr)
		return E_FAIL;

	return m_pFrame_Mgr->Add_Frame(pFrameTag, fCallCnt);
}

_bool CSystem::Permit_Call(const _tchar * pFrameTag, const _float & fTimeDelta)
{
	if (nullptr == m_pFrame_Mgr)
		return false;

	return m_pFrame_Mgr->Permit_Call(pFrameTag, fTimeDelta);
}

void CSystem::Free()
{
	Safe_Release(m_pTimer_Mgr);
	Safe_Release(m_pFrame_Mgr);
}
