#include "framework.h"
#include "Light_Manager.h"
#include "Light.h"
_IMPLEMENT_SINGLETON(CLight_Manager)
CLight_Manager::CLight_Manager()
{

}
LIGHT CLight_Manager::GetLight(const _tchar* pLightTag)
{
	auto iter_find = m_mapLightInfo.find(pLightTag);
	if (iter_find == m_mapLightInfo.end())
		return LIGHT();

	return (*iter_find).second->Get_LightInfo();
}

HRESULT CLight_Manager::Add_LightInfo(ID3D12Device* pGraphic_Device, 
	const _tchar* pLightTag, LIGHT& tLightInfo)
{
	auto iter_find = m_mapLightInfo.find(pLightTag);
	if (iter_find != m_mapLightInfo.end())
		return E_FAIL;

	CLight* pInstance = CLight::Create(pGraphic_Device, tLightInfo);
	if (nullptr == pInstance)
		return E_FAIL;

	m_mapLightInfo.insert(make_pair(pLightTag, pInstance));

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& iter : m_mapLightInfo)
		Safe_Release(iter.second);
	m_mapLightInfo.clear();
}
