#include "stdafx.h"
#include "ObjMgr.h"
#include "Obj.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
}


CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::AddObject(CObj* pObj, OBJID eID)
{
	m_ObjLst[eID].push_back(pObj);
}

void CObjMgr::Update()
{
	for (int i = 0; i < END; ++i)
	{
		OBJITER iter_begin = m_ObjLst[i].begin();

		for (; iter_begin != m_ObjLst[i].end();)
		{
			int iEvent = (*iter_begin)->Update();

			if (DEAD_OBJ == iEvent)
			{
				SafeDelete(*iter_begin);
				iter_begin = m_ObjLst[i].erase(iter_begin);
			}
			else
				++iter_begin;
		}
	}	
}

void CObjMgr::LateUpdate()
{
	for (int i = 0; i < END; ++i)
	{
		for (auto& pObject : m_ObjLst[i])
			pObject->LateUpdate();
	}
}

void CObjMgr::Render()
{
	for (int i = 0; i < END; ++i)
	{
		for (auto& pObject : m_ObjLst[i])
			pObject->Render();
	}
}

void CObjMgr::Release()
{
	for (int i = 0; i < END; ++i)
	{
		for_each(m_ObjLst[i].begin(), m_ObjLst[i].end(), SafeDelete<CObj*>);
		m_ObjLst[i].clear();
	}	
}
