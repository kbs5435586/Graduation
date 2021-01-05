#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"



CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}


HRESULT CGameObject::Ready_GameObject(void* pArg)
{
	return S_OK;
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CGameObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return _int();
}

void CGameObject::Render_GameObject()
{
}

CComponent * CGameObject::Get_ComponentPointer(const _tchar * pComponentTag)
{
	CComponent*	pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Add_Component(const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	m_mapComponent.insert({ pComponentTag, pComponent });

	pComponent->AddRef();

	return S_OK;
}


HRESULT CGameObject::Delete_Component(const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(pComponentTag))
		m_mapComponent.erase(pComponentTag);
	Safe_Release(pComponent);
	return S_OK;
}


CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CFinder_Tag(pComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}


void CGameObject::Free()
{
	for (auto& Pair : m_mapComponent)
		Safe_Release(Pair.second);
	m_mapComponent.clear();

	Safe_Release(m_pGraphic_Device);
}
