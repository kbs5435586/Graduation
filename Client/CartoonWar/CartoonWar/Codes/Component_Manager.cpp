#include "framework.h"
#include "Component_Manager.h"
#include "Component.h"
#include "Layer.h"
#include "GameObject.h"

_IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{

}
HRESULT CComponent_Manager::Reserve_Component_Manager(const _uint& iNumScene)
{
	if (nullptr != m_pMapPrototype)
		return E_FAIL;

	m_pMapPrototype = new MAPPROTOTYPE[iNumScene];

	m_iNumScene = iNumScene;


	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent)
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

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg)
{
	CComponent* pPrototype = Find_Component(iSceneID, pComponentTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pClone = pPrototype->Clone_Component(pArg);
	if (nullptr == pClone)
		return nullptr;

	return pClone;
}

CComponent* CComponent_Manager::Find_Component(const _uint& iSceneID, const _tchar* pComponentTag)
{
	auto	iter = find_if(m_pMapPrototype[iSceneID].begin(), m_pMapPrototype[iSceneID].end(), CFinder_Tag(pComponentTag));

	if (iter == m_pMapPrototype[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pMapPrototype[i])
			Safe_Release(Pair.second);

		m_pMapPrototype[i].clear();
	}

	Safe_Delete_Array(m_pMapPrototype);
}
