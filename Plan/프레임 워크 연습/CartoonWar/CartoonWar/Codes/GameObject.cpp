#include "framework.h"
#include "GameObject.h"
#include "Component.h"

CGameObject::CGameObject()
{

}
CGameObject::CGameObject(const CGameObject& rhs)
{
}

CComponent* CGameObject::Get_ComponentPointer(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

//HRESULT CGameObject::Ready_GameObject(void* pArg)
//{
//
//	return S_OK;
//}

HRESULT CGameObject::Ready_GameObject(void* pArg, void* pSize)
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

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CFinder_Tag(pComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

CGameObject* CGameObject::Clone_GameObject(void* pArg)
{
	return nullptr;
}

CGameObject* CGameObject::Clone_GameObject(void* pArg, void* pSize)
{
	return nullptr;
}

void CGameObject::Free()
{
	for (auto& Pair : m_mapComponent)
		Safe_Release(Pair.second);
	m_mapComponent.clear();
}
