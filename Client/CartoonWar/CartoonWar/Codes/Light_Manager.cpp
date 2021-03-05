#include "framework.h"
#include "Light_Manager.h"
#include "Light.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CLight_Manager)
CLight_Manager::CLight_Manager()
{

}
LIGHT CLight_Manager::GetLight(const _tchar* pLightTag)
{
	auto iter_find = m_mapLightInfo.find(pLightTag);
	if (iter_find == m_mapLightInfo.end())
		return LIGHT();

	return iter_find->second->Get_LightInfo();
}

HRESULT CLight_Manager::Add_LightInfo(const _tchar* pLightTag, LIGHT& tLightInfo)
{
	auto iter_find = m_mapLightInfo.find(pLightTag);
	if (iter_find != m_mapLightInfo.end())
		return E_FAIL;

	CLight* pInstance = CLight::Create(tLightInfo);
	if (nullptr == pInstance)
		return E_FAIL;

	m_mapLightInfo.insert(make_pair(pLightTag, pInstance));

	return S_OK;
}



void CLight_Manager::SetUp_OnShader()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	LIGHTINFO tInfo = {};



	if (m_mapLightInfo.size() <= 0)
	{
		Safe_Release(pManagement);
		return;
	}

	for (auto& iter : m_mapLightInfo)
	{
		m_vecLightInfo.push_back(iter.second);
	}


	for (int i = 0; i < m_vecLightInfo.size(); ++i)
	{
		LIGHT tLight = m_vecLightInfo[i]->Get_LightInfo();
		tInfo.arrLight[i] = tLight;
	}
	tInfo.iCurLightCnt = (UINT)m_vecLightInfo.size();


	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b2)->SetData(&tInfo);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b2)->GetCBV().Get(), iOffset, CONST_REGISTER::b2);


	m_vecLightInfo.clear();
	Safe_Release(pManagement);
}

void CLight_Manager::Free()
{
	for (auto& iter : m_mapLightInfo)
		Safe_Release(iter.second);
	m_mapLightInfo.clear();
}
