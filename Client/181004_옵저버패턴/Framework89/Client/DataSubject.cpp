#include "stdafx.h"
#include "DataSubject.h"

IMPLEMENT_SINGLETON(CDataSubject)

CDataSubject::CDataSubject()
{
}


CDataSubject::~CDataSubject()
{
	Release();
}

list<void*>* CDataSubject::GetDataLst(int iMessage)
{
	auto& iter = m_MapData.find(iMessage);

	if (m_MapData.end() == iter)
		return nullptr;

	return &(iter->second);
}

void CDataSubject::Release()
{
	for (auto& Mypair : m_MapData)
		Mypair.second.clear();

	m_MapData.clear();
}

void CDataSubject::AddData(int iMessage, void* pData)
{
	if (nullptr != pData)
	{
		m_MapData[iMessage].push_back(pData);
		CSubject::Notify(iMessage, pData);
	}
}

void CDataSubject::RemoveData(int iMessage, void* pData)
{
	auto MapIter = m_MapData.find(iMessage);

	if (m_MapData.end() != MapIter)
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
