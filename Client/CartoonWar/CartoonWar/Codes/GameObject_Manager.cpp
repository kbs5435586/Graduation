#include "framework.h"
#include "GameObject_Manager.h"
#include "Layer.h"
#include "GameObject.h"
_IMPLEMENT_SINGLETON(CGameObject_Manager)
CGameObject_Manager::CGameObject_Manager()
{

}
CComponent* CGameObject_Manager::Get_ComponentPointer(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex)
{
	if (nullptr == m_pMapLayers)
		return nullptr;

	if (m_iNumScene <= iSceneID)
		return nullptr;

	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPointer(pComponentTag, iIndex);
}
HRESULT CGameObject_Manager::Reserve_Object_Manager(const _uint& iNumScene)
{
	if (nullptr != m_pMapLayers)
		return E_FAIL;

	m_pMapLayers = new MAPLAYERS[iNumScene];

	m_iNumScene = iNumScene;

	return S_OK;
}

HRESULT CGameObject_Manager::Add_Prototype_GameObject(const _tchar* pGameObjectTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr != Find_Prototype(pGameObjectTag))
		return E_FAIL;

	m_mapPrototype.insert(MAPPROTOTYPE::value_type(pGameObjectTag, pGameObject));

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObjectToLayer(const _tchar* pProtoTag, const _uint& iSceneID, const _tchar* pLayerTag, CGameObject** ppCloneObject, void* pArg, _uint iIdx)
{
	if (nullptr == m_pMapLayers)
		return E_FAIL;

	if (m_iNumScene <= iSceneID)
		return E_FAIL;

	CGameObject* pPrototype = nullptr;

	pPrototype = Find_Prototype(pProtoTag);

	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone_GameObject(pArg, iIdx);
	pGameObject->m_iLayerIdx = iIdx;
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr != ppCloneObject)
		*ppCloneObject = pGameObject;

	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer) 
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_Object(pGameObject)))
			return E_FAIL;

		m_pMapLayers[iSceneID].insert(MAPLAYERS::value_type(pLayerTag, pLayer));
	}
	else // 레이거가 이미 있었어.
		if (FAILED(pLayer->Add_Object(pGameObject)))
			return E_FAIL;

	return S_OK;
}

_int CGameObject_Manager::Update_GameObject_Manager(const _float& fTimeDelta)
{
	if (nullptr == m_pMapLayers)
		return -1;

	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return -1;
	server->AddRef();

	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapLayers[i])
		{
			if (nullptr != Pair.second)
			{
				server->EventManager();
				if (Pair.second->Update_Object(fTimeDelta) & 0x80000000)
					return -1;
			}
		}
	}
	Safe_Release(server);
	return _int();
}

_int CGameObject_Manager::LastUpdate_GameObject_Manager(const _float& fTimeDelta)
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

HRESULT CGameObject_Manager::Clear_Layers(const _uint& iSceneID)
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

CGameObject* CGameObject_Manager::Get_BackObject(const _uint& iSceneID, const _tchar* pLayerTag)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_BackObject();
}

CGameObject* CGameObject_Manager::Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIdx)
{
	if (nullptr == m_pMapLayers)
		return nullptr;
	if (m_iNumScene <= iSceneID)
		return nullptr;

	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIdx);
}

list<CGameObject*> CGameObject_Manager::Get_GameObjectLst(const _uint& iSceneID, const _tchar* pLayerTag)
{
	auto iter = find_if(m_pMapLayers[iSceneID].begin(), m_pMapLayers[iSceneID].end()
		, CFinder_Tag(pLayerTag));

	if (iter == m_pMapLayers[iSceneID].end())
	{
		return list<CGameObject*>();
	}

	return iter->second->Get_GameObjectLst();
}

CGameObject* CGameObject_Manager::Find_Prototype(const _tchar* pGameObjectTag)
{
	auto	iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CFinder_Tag(pGameObjectTag));

	if (iter == m_mapPrototype.end())
		return nullptr;

	return iter->second;
}

CLayer* CGameObject_Manager::Find_Layer(const _uint& iSceneID, const _tchar* pLayerTag)
{
	auto	iter = find_if(m_pMapLayers[iSceneID].begin(), m_pMapLayers[iSceneID].end(), CFinder_Tag(pLayerTag));

	if (iter == m_pMapLayers[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CGameObject_Manager::Free()
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
