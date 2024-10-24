#include "framework.h"
#include "Light_Manager.h"
#include "Light.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CLight_Manager)
CLight_Manager::CLight_Manager()
{

}

HRESULT CLight_Manager::Add_LightInfo(LIGHT& tLightInfo)
{
	CLight* pInstance = CLight::Create(tLightInfo);
	if (nullptr == pInstance)
		return E_FAIL;

	m_vecLightInfo.push_back(pInstance);

	return S_OK;
}



LIGHT CLight_Manager::GetLight(_uint& iIdx)
{
	if (iIdx < 0)
		return LIGHT();

	return m_vecLightInfo[iIdx]->Get_LightInfo();
}

void CLight_Manager::SetUp_OnShader()
{
	LIGHTINFO tInfo = {};

	for (int i = 0; i < m_vecLightInfo.size(); ++i)
	{
		LIGHT tLight = m_vecLightInfo[i]->Get_LightInfo();
		tInfo.arrLight[i] = tLight;
	}
	tInfo.iCurLightCnt = (UINT)m_vecLightInfo.size();

	_uint iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b2)->SetData(&tInfo);
	CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b2)->GetCBV().Get(),
		iOffset, CONST_REGISTER::b2);
}

void CLight_Manager::Update_DiffuseLight(const _float& fTimeDelata)
{
	m_vecLightInfo[0]->Get_LightInfo().vLightDir.x = g_iDiffusePer;

	if (g_iDiffusePer <= 1.f && g_iDiffusePer >= 0.75f)
	{
		m_vecLightInfo[0]->Get_LightInfo().tLightColor.vDiffuse = _vec4(0.5f, 0.5f, 0.5f, 0.f);
	}
	else if (g_iDiffusePer <= 0.75f && g_iDiffusePer >= -0.75f)
	{
		m_vecLightInfo[0]->Get_LightInfo().tLightColor.vDiffuse = _vec4(1.f, 1.f, 1.f, 0.f);
	}
	else if (g_iDiffusePer <= -0.75f && g_iDiffusePer >= -1.f)
	{
		m_vecLightInfo[0]->Get_LightInfo().tLightColor.vDiffuse = _vec4(1.f, 0.5f, 0.f, 0.f);
	}
	else if (g_iDiffusePer <= -1.f)
	{
		m_vecLightInfo[0]->Get_LightInfo().tLightColor.vDiffuse = _vec4(0.f, 0.f, 0.f,0.f);
		m_vecLightInfo[0]->Get_LightInfo().tLightColor.vSpecular = _vec4(0.f, 0.f, 0.f, 0.f);
	}
}

void CLight_Manager::Update()
{
	for (int i = 0; i < m_vecLightInfo.size(); ++i)
	{
		m_vecLightInfo[i]->Update();
		m_vecLightInfo[i]->Set_ArrIdx(i);
	}
}

void CLight_Manager::Render()
{
	for (int i = 0; i < m_vecLightInfo.size(); ++i)
	{
		m_vecLightInfo[i]->Render();
	}
}

void CLight_Manager::Free()
{
	for (auto& iter : m_vecLightInfo)
	{
		Safe_Release(iter);
	}
}