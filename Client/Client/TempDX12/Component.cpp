#include "framework.h"
#include "Component.h"

CComponent::CComponent(ID3D12Device* pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

CComponent::CComponent(const CComponent& rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
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
