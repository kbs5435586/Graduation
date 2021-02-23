#include "stdafx.h"
#include "Subject.h"
#include "Observer.h"

CSubject::CSubject()
{
}


CSubject::~CSubject()
{
}

void CSubject::Release()
{
	m_ObserverLst.clear();
}

void CSubject::Subscribe(CObserver* pObserver)
{
	m_ObserverLst.push_back(pObserver);
}

void CSubject::UnSubscribe(CObserver* pObserver)
{
	list<CObserver*>::iterator iter = m_ObserverLst.begin();

	for (; iter != m_ObserverLst.end();)
	{
		if (*iter == pObserver)
			m_ObserverLst.erase(iter);
		else
			++iter;
	}
}

void CSubject::Notify(int iMessage, void* pData)
{
	list<CObserver*>::iterator iter = m_ObserverLst.begin();

	for (; iter != m_ObserverLst.end(); ++iter)
		(*iter)->Update(iMessage, pData);
}
