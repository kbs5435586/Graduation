#include "stdafx.h"
#include "Light.h"

CLight::CLight(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CLight::Ready_Light(const D3DLIGHT9 & LightInfo)
{
	m_LightInfo = LightInfo;

	return NOERROR;
}

CLight * CLight::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightInfo)
{
	CLight*	pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Ready_Light(LightInfo)))
	{
		MessageBox(0, L"CLight Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pGraphic_Device);
}