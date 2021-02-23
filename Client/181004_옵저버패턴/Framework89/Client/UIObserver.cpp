#include "stdafx.h"
#include "UIObserver.h"


CUIObserver::CUIObserver()
{
}


CUIObserver::~CUIObserver()
{
}

void CUIObserver::Update(int iMessage, void* pData)
{
	// pull 옵저버
	list<void*>* pDataLst = GET_INSTANCE(CDataSubject)->GetDataLst(iMessage);

	if (nullptr == pDataLst)
		return;

	// find(begin, end, data): 컨테이너를 순회하면서 data를 찾는다. <algorithm>에서 제공.
	auto iter_find = find(pDataLst->begin(), pDataLst->end(), pData);

	if (pDataLst->end() == iter_find)
		return;

	switch (iMessage)
	{
	case CDataSubject::PLAYER_DATA:
		m_tStat = *reinterpret_cast<STAT*>(*iter_find);
		break;
	case CDataSubject::PLAYER_MATRIX:
		break;
	}
}
