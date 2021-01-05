#include "stdafx.h"
#include "Layer.h"


CLayer::CLayer()
{
}

HRESULT CLayer::Add_Object(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Object(const _float & fTimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->Update_GameObject(fTimeDelta) & 0x80000000)
				return -1;
		}
	}

	return _int();
}

_int CLayer::LastUpdate_Object(const _float & fTimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->LastUpdate_GameObject(fTimeDelta) & 0x80000000)
				return -1;
		}
	}

	return _int();
}

CComponent * CLayer::Get_ComponentPointer(const _tchar * pComponentTag, const _uint & iIdx)
{
	if (m_ObjectList.size() <= iIdx)
		return nullptr;
	auto& iter = m_ObjectList.begin();
	for (size_t i = 0; i < iIdx; ++i)
		++iter;

	return (*iter)->Get_ComponentPointer(pComponentTag);
}

list<CGameObject*>* CLayer::GetGameObjectList()
{
	return &m_ObjectList;
}

CGameObject * CLayer::Get_GameObject(const _uint & iIndex)
{
	if (m_ObjectList.size() <= iIndex)
		return nullptr;
	auto	iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter);
}

CGameObject * CLayer::Get_BackObject()
{
	return m_ObjectList.back();
}


CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MessageBox(0, L"CLayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
	{
		Safe_Release(pGameObject);
	}
	m_ObjectList.clear();
}
