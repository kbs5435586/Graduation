#include "stdafx.h"
#include "Light_Mgr.h"
#include "Light.h"
_IMPLEMENT_SINGLETON(CLight_Mgr)

CLight_Mgr::CLight_Mgr()
{
}

D3DLIGHT9 * CLight_Mgr::Get_LightInfo(const _uint & iIndex)
{
	if (m_LightList.size() <= iIndex)
		return nullptr;

	auto	iter = m_LightList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightInfo();
}

HRESULT CLight_Mgr::Add_LightInfo(LPDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightInfo)
{
	CLight*			pLight = CLight::Create(pGraphic_Device, LightInfo);
	if (nullptr == pLight)
		return E_FAIL;

	m_LightList.push_back(pLight);

	return NOERROR;
}

void CLight_Mgr::Free()
{
	for (auto& pLight : m_LightList)
		Safe_Release(pLight);
	m_LightList.clear();
}
