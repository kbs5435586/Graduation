#include "stdafx.h"
#include "Timer_Mgr.h"
#include "Timer.h"

_IMPLEMENT_SINGLETON(CTimer_Mgr)
CTimer_Mgr::CTimer_Mgr()
{
}


HRESULT CTimer_Mgr::Add_Timer(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (pTimer != nullptr)
		return E_FAIL;
	pTimer = CTimer::Create();
	if (pTimer == nullptr)
		return E_FAIL;

	m_mapTImer.insert({pTimerTag, pTimer});
	return S_OK;
}

_float CTimer_Mgr::Get_TimeDelta(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;


	return pTimer->Get_TimeDelta();
}

CTimer * CTimer_Mgr::Find_Timer(const _tchar * pTimerTag)
{
	auto iter = find_if(m_mapTImer.begin(), m_mapTImer.end(), CFinder_Tag(pTimerTag));

	if (iter == m_mapTImer.end())
		return nullptr;

	return iter->second;
}

void CTimer_Mgr::Free()
{
	_ulong	dwRefCnt = 0;
	for(auto& Pair:m_mapTImer)
	{
		if (dwRefCnt = Safe_Release(Pair.second))
			_MSG_BOX("Timer Release Failed");
	}
	m_mapTImer.clear();
}
