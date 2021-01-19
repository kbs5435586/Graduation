#include "framework.h"
#include "Light.h"

CLight::CLight()
{

}

HRESULT CLight::Ready_Light(const LIGHT& tLightInfo)
{
	m_pLight = new LIGHT(tLightInfo);

	return S_OK;
}

CLight* CLight::Create(const LIGHT& tLightInfo)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Ready_Light(tLightInfo)))
	{
		MessageBox(0, L"CLight Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Delete(m_pLight);
}
