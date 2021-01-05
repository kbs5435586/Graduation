#include "stdafx.h"
#include "Component_Mgr.h"

_IMPLEMENT_SINGLETON(CComponent_Mgr)
CComponent_Mgr::CComponent_Mgr()
{
}

HRESULT CComponent_Mgr::Reserve_Component_Manager(const _uint & iNumScene)
{
	if (nullptr != m_pMapPrototype)
		return E_FAIL;

	m_pMapPrototype = new MAPPROTOTYPE[iNumScene];

	m_iNumScene = iNumScene;

	return S_OK;
}

HRESULT CComponent_Mgr::Add_Prototype_Component(const _uint & iSceneID, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr == m_pMapPrototype)
		return E_FAIL;

	if (m_iNumScene <= iSceneID)
		return E_FAIL;

	if (nullptr != Find_Component(iSceneID, pComponentTag))
		return E_FAIL;

	m_pMapPrototype[iSceneID].insert(MAPPROTOTYPE::value_type(pComponentTag, pComponent));

	CComponent* pPrototype = Find_Component(iSceneID, pComponentTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	return S_OK;
}

HRESULT CComponent_Mgr::Delete_Prototype_Component(const _uint & iSceneID, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr == m_pMapPrototype)
		return E_FAIL;

	if (m_iNumScene <= iSceneID)
		return E_FAIL;

	CComponent* find_Comp = Find_Component(iSceneID, pComponentTag);
	if (nullptr == find_Comp)
		return E_FAIL;
	
	m_pMapPrototype[iSceneID].erase(pComponentTag);
}

CComponent * CComponent_Mgr::Clone_Component(const _uint & iSceneID, const _tchar * pComponentTag, void* pArg)
{
	CComponent* pPrototype = Find_Component(iSceneID, pComponentTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pClone = pPrototype->Clone_Component(pArg);
	if (nullptr == pClone)
		return nullptr;

	return pClone;
}

CComponent * CComponent_Mgr::Find_Component(const _uint & iSceneID, const _tchar * pComponentTag)
{
	auto	iter = find_if(m_pMapPrototype[iSceneID].begin(), m_pMapPrototype[iSceneID].end(), CFinder_Tag(pComponentTag));

	if (iter == m_pMapPrototype[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CComponent_Mgr::Free()
{
	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapPrototype[i])
			Safe_Release(Pair.second);

		m_pMapPrototype[i].clear();
	}

	Safe_Delete_Array(m_pMapPrototype);
}
