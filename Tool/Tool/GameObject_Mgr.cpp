#include "stdafx.h"
#include "GameObject_Mgr.h"
#include "Layer.h"
_IMPLEMENT_SINGLETON(CGameObject_Mgr)
CGameObject_Mgr::CGameObject_Mgr()
{
}

HRESULT CGameObject_Mgr::Reserve_Object_Manager(const _uint & iNumScene)
{
	if (nullptr != m_pMapLayers)
		return E_FAIL;

	m_pMapLayers = new MAPLAYERS[iNumScene];

	m_iNumScene = iNumScene;

	return S_OK;
}

HRESULT CGameObject_Mgr::Add_Prototype_GameObject(const _tchar * pGameObjectTag, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr != Find_Prototype(pGameObjectTag))
		return E_FAIL;

	m_mapPrototype.insert(MAPPROTOTYPE::value_type(pGameObjectTag, pGameObject));

	return S_OK;
}

HRESULT CGameObject_Mgr::Add_GameObjectToLayer(const _tchar * pProtoTag, const _uint & iSceneID, const _tchar * pLayerTag, void* pArg, CGameObject ** ppCloneObject)
{
	if (nullptr == m_pMapLayers)
		return E_FAIL;

	if (m_iNumScene <= iSceneID)
		return E_FAIL;

	CGameObject*	pPrototype = nullptr;

	pPrototype = Find_Prototype(pProtoTag);

	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone_GameObject(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr != ppCloneObject)
		*ppCloneObject = pGameObject;

	CLayer*	pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_Object(pGameObject)))
			return E_FAIL;

		m_pMapLayers[iSceneID].insert(MAPLAYERS::value_type(pLayerTag, pLayer));
	}
	else 
		if (FAILED(pLayer->Add_Object(pGameObject)))
			return E_FAIL;

	return S_OK;
}


_int CGameObject_Mgr::Update_GameObject_Manager(const _float & fTimeDelta)
{
	if (nullptr == m_pMapLayers)
		return -1;

	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapLayers[i])
		{
			if (nullptr != Pair.second)
			{
				if (Pair.second->Update_Object(fTimeDelta) & 0x80000000)
					return -1;
			}
		}
	}
	

	return _int();
}

_int CGameObject_Mgr::LastUpdate_GameObject_Manager(const _float & fTimeDelta)
{
	if (nullptr == m_pMapLayers)
		return -1;

	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapLayers[i])
		{
			if (nullptr != Pair.second)
			{
				if (Pair.second->LastUpdate_Object(fTimeDelta) & 0x80000000)
					return -1;
			}
		}
	}

	return _int();
}

HRESULT CGameObject_Mgr::Clear_Layers(const _uint & iSceneID)
{
	if (nullptr == m_pMapLayers)
		return E_FAIL;

	if (m_iNumScene <= iSceneID)
		return E_FAIL;

	for (auto& Pair : m_pMapLayers[iSceneID])
		Safe_Release(Pair.second);

	m_pMapLayers[iSceneID].clear();

	return S_OK;
}

CGameObject * CGameObject_Mgr::Find_Prototype(const _tchar * pGameObjectTag)
{
	auto	iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CFinder_Tag(pGameObjectTag));
	if (iter == m_mapPrototype.end())
		return nullptr;
	return iter->second;
}

CLayer * CGameObject_Mgr::Find_Layer(const _uint & iSceneID, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pMapLayers[iSceneID].begin(), m_pMapLayers[iSceneID].end(), CFinder_Tag(pLayerTag));
	if (iter == m_pMapLayers[iSceneID].end())
		return nullptr;
	return iter->second;
}

CComponent * CGameObject_Mgr::Get_ComponentPointer(const _uint & iSceneID, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & iIdx)
{
	if (nullptr == m_pMapLayers)
		return nullptr;

	if (m_iNumScene <= iSceneID)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPointer(pComponentTag, iIdx);
}

list<CGameObject*>* CGameObject_Mgr::Get_ObjectList(const _uint & iSceneID, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pMapLayers[iSceneID].begin(), m_pMapLayers[iSceneID].end(), CFinder_Tag(pLayerTag));

	if (iter == m_pMapLayers[iSceneID].end())
	{
		return nullptr;
	}

	return iter->second->GetGameObjectList();
}

map<const _tchar*, CLayer*>* CGameObject_Mgr::Get_MapLayer(const _uint & iSceneID)
{
	return &m_pMapLayers[iSceneID];
}


CGameObject * CGameObject_Mgr::Get_GameObject(const _uint & iSceneID, const _tchar * pLayerTag, const _uint & iIndex)
{
	if (nullptr == m_pMapLayers)
		return nullptr;
	if (m_iNumScene <= iSceneID)
		return nullptr;
	CLayer*		pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIndex);
}

CGameObject * CGameObject_Mgr::Get_BackObject(const _uint & iSceneID, const _tchar * pLayerTag)
{
	CLayer*	pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_BackObject();
}


HRESULT CGameObject_Mgr::Find_Clear_Layer(const _uint& iSceneID, const _tchar * pLayerTag)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return E_FAIL;

	for (auto& iter : *pLayer->GetGameObjectList())
		Safe_Release(iter);
	
	pLayer->GetGameObjectList()->clear();

	return S_OK;
}



void CGameObject_Mgr::Free()
{
	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapLayers[i])
			Safe_Release(Pair.second);

		m_pMapLayers[i].clear();
	}

	Safe_Delete_Array(m_pMapLayers);

	for (auto& Pair : m_mapPrototype)
		Safe_Release(Pair.second);

	m_mapPrototype.clear();
}
