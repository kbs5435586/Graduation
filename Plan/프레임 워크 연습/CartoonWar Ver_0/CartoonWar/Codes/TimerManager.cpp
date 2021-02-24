#include "framework.h"
#include "TimerManager.h"
#include "Timer.h"

_IMPLEMENT_SINGLETON(CTimerManager);
CTimerManager::CTimerManager()
{

}
HRESULT CTimerManager::Add_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.insert(MAPTIMERS::value_type(pTimerTag, pTimer));

	return S_OK;
}

_float CTimerManager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;
	return pTimer->Get_TimeDelta();
}

CTimer* CTimerManager::Find_Timer(const _tchar* pTimerTag)
{
	auto iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), 
		[&](MAPTIMERS::value_type Pair)->bool {return !lstrcmp(Pair.first, pTimerTag); });

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimerManager::Free(void)
{
	_ulong		dwRefCnt = 0;

	for (auto& Pair : m_mapTimers)
	{
		if (dwRefCnt = Safe_Release(Pair.second))
			_MSG_BOX("CTimer Release Failed");
	}

	m_mapTimers.clear();
}
