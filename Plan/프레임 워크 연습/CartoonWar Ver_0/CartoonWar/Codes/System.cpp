#include "framework.h"
#include "System.h"
#include "FrameManager.h"
#include "TimerManager.h"

_IMPLEMENT_SINGLETON(CSystem);
CSystem::CSystem()
	: m_pTimer_Manager(CTimerManager::GetInstance())
	, m_pFrame_Manager(CFrameManager::GetInstance())
{
	m_pTimer_Manager->AddRef();
	m_pFrame_Manager->AddRef();

}

HRESULT CSystem::Add_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

_float CSystem::Get_TimeDelta(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CSystem::Add_Frame(const _tchar* pFrameTag, const _float& fCallCnt)
{
	if (nullptr == m_pFrame_Manager)
		return E_FAIL;

	return m_pFrame_Manager->Add_Frame(pFrameTag, fCallCnt);
}

_bool CSystem::Permit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	if (nullptr == m_pFrame_Manager)
		return false;

	return m_pFrame_Manager->Permit_Call(pFrameTag, fTimeDelta);
}

void CSystem::Free()
{
	Safe_Release(m_pFrame_Manager);
	Safe_Release(m_pTimer_Manager);
}
