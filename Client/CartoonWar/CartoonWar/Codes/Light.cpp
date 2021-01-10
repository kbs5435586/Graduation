#include "framework.h"
#include "Light.h"

CLight::CLight(ID3D12Device* pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;
	m_pGraphic_Device->AddRef();
}

HRESULT CLight::Ready_Light(const LIGHT& tLightInfo)
{
	m_tLight = tLightInfo;
	return S_OK;
}

CLight* CLight::Create(ID3D12Device* pGraphic_Device, const LIGHT& tLightInfo)
{
	CLight* pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Ready_Light(tLightInfo)))
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
