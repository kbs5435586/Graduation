#include "framework.h"
#include "ObserverManager.h"
#include "Observer.h"


_IMPLEMENT_SINGLETON(CObserverManager)

CObserverManager::CObserverManager()
{
}

HRESULT CObserverManager::Ready_ObserverManager()
{
	return S_OK;
}

void CObserverManager::Add_Data(DATA_TYPE eType, void* pData)
{
	if (nullptr == pData)
		return;

	m_mapData[eType].push_back(pData);

	Notify(eType, pData);
}

void CObserverManager::Remove_Data(DATA_TYPE eType, void* pData)
{
	auto MapIter = m_mapData.find(eType);

	if (m_mapData.end() != MapIter)
	{
		auto iter_begin = MapIter->second.begin();

		for (; iter_begin != MapIter->second.end(); )
		{
			if (*iter_begin == pData)
				iter_begin = MapIter->second.erase(iter_begin);
			else
				iter_begin++;
		}
	}
}

list<void*>* CObserverManager::Get_List(DATA_TYPE eType)
{
	auto iter = m_mapData.find(eType);

	if(iter == m_mapData.end())
		return nullptr;

	return &iter->second;
}

CObserverManager* CObserverManager::Create()
{
	CObserverManager* pInstance = new CObserverManager();
	if (FAILED(pInstance->Ready_ObserverManager()))
		Safe_Release(pInstance);
	return pInstance;
}

void CObserverManager::Free()
{
	for (auto& iter : m_lstSubject)
	{
		if (iter)
		{
			Safe_Release(iter);
		}
	}
}

void CObserverManager::Subscribe(CObserver* pObserver)
{
	m_lstSubject.push_back(pObserver);
}

void CObserverManager::UnSubscribe(CObserver* pObserver)
{
	auto iter = m_lstSubject.begin();

	for (; iter != m_lstSubject.end();)
	{
		if (*iter == pObserver)
			m_lstSubject.erase(iter);
		else
			++iter;
	}
}

void CObserverManager::Notify(DATA_TYPE eType, void* pData)
{
	list<CObserver*>::iterator iter = m_lstSubject.begin();

	for (; iter != m_lstSubject.end(); ++iter)
		(*iter)->Update_Observer(eType, pData);

}

void CObserverManager::ReNotify(DATA_TYPE eType)
{
	list<CObserver*>::iterator iter = m_lstSubject.begin();

	for (; iter != m_lstSubject.end(); ++iter)
		(*iter)->ReUpdate_Observer(eType);
}
