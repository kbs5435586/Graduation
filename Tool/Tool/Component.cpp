#include "stdafx.h"
#include "Component.h"

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_isClone(false)
{
	m_pGraphic_Device->AddRef();
}

CComponent::CComponent(const CComponent & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_isClone(true)
	, m_pGameObjectName(rhs.m_pGameObjectName)
	, m_pObject_FileName(rhs.m_pObject_FileName)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CComponent::Ready_Component()
{
	return S_OK;
}


void CComponent::Free()
{
	Safe_Release(m_pGraphic_Device);
}
