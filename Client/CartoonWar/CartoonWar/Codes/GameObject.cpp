#include "framework.h"
#include "GameObject.h"
#include "Component.h"

CGameObject::CGameObject()
{

}
CGameObject::CGameObject(const CGameObject& rhs)
	: m_iLayerIdx(rhs.m_iLayerIdx)
	, m_fBazierCnt(rhs.m_fBazierCnt)
	, m_IsBazier(rhs.m_IsBazier)
	, m_IsOBB_Collision(rhs.m_IsOBB_Collision)
{
}

CComponent* CGameObject::Get_ComponentPointer(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Ready_GameObject(void* pArg)
{

	return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CGameObject::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CGameObject::Render_GameObject()
{
}

void CGameObject::Render_GameObject_Shadow()
{
}

void CGameObject::Render_PostEffect()
{
}

void CGameObject::Render_Blur()
{
}

void CGameObject::Render_Ref()
{
}

void CGameObject::Render_GameObject_Map()
{
}

HRESULT CGameObject::CreateInputLayout()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	m_mapComponent.insert(MAPCOMPONENT::value_type(pComponentTag, pComponent));

	pComponent->AddRef();

	return S_OK;
}

HRESULT CGameObject::Delete_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	if(nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(pComponentTag))
	{

		auto iter = m_mapComponent.find(pComponentTag);
		m_mapComponent.erase(iter);
		//Safe_Release(pComponent);
	}

	return S_OK;
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CFinder_Tag(pComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

CGameObject* CGameObject::Clone_GameObject(void* pArg, _uint iIdx)
{
	return nullptr;
}

void CGameObject::Free()
{
	for (auto& Pair : m_mapComponent)
		Safe_Release(Pair.second);
	m_mapComponent.clear();
}

void CGameObject::SetComponentTag(_tchar* pTag)
{
	lstrcpy(m_pComponentTag, pTag);
}
