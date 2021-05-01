#include "framework.h"
#include "Light.h"
#include "Shader.h"
#include "Transform.h"
#include "Management.h"
CLight::CLight()
{

}

HRESULT CLight::Ready_Light(LIGHT& tLightInfo)
{
	m_tLight = tLightInfo;

	m_pTransformCom = CTransform::Create();
	m_pShader_Dir = CShader::Create(L"../ShaderFiles/Shader_Deffered_Light.hlsl", "VS_DirLight", "PS_DirLight");
	m_pShader_Point = CShader::Create(L"../ShaderFiles/Shader_Deffered_Light.hlsl", "VS_PointLight", "PS_PointLight");
	m_pBufferCom = CBuffer_RectTex::Create();



	_vec3 vSize = { tLightInfo.fRange,tLightInfo.fRange ,tLightInfo.fRange };
	_vec3 vPos = { tLightInfo.vLightPos };

	m_pTransformCom->Scaling(vSize);
	tLightInfo.vLightDir.Normalize();

	if (FAILED(CreateInputLayout(m_tLight.iLightType)))
		return E_FAIL;

	

	return S_OK;
}

void CLight::Update()
{

	_vec4 vPos = { m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->x,
		   m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->y,
			m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->z, 1.f };
	m_tLight.vLightPos = vPos;

	m_pTransformCom->Scaling(m_tLight.fRange, m_tLight.fRange, m_tLight.fRange);
}

void CLight::Render()
{
	MAINPASS tMainPass = {};
	REP tRep = {};
	tRep.m_arrInt[0] = m_iArridx;

	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();
	// 
	//_matrix matWorld = Matrix_::Identity();
	//_matrix matView = Matrix_::Identity();
	//_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();
	//
	//matWorld._11 = 800.f;
	//matWorld._22 = 600.f;
	//
	//matWorld._41 = 400.f - (WINCX >> 1);
	//matWorld._42 = -300.f + (WINCY >> 1);

	if (m_tLight.iLightType == 0)
	{
		m_pShader_Dir->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	}
	else if (m_tLight.iLightType == 1)
	{
		m_pShader_Point->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	}

	_uint iOffeset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer(
		(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer(
		(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);



	ComPtr<ID3D12DescriptorHeap>	pNormalTex = CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(1)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pNormalTex.Get(), TEXTURE_REGISTER::t0);

	ComPtr<ID3D12DescriptorHeap>	pPositionTex = CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(2)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pPositionTex.Get(), TEXTURE_REGISTER::t1);

	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();
}

HRESULT CLight::CreateInputLayout(_uint iType)
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (iType == 0)
	{
		if (FAILED(m_pShader_Dir->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_LIGHT)))
			return E_FAIL;
	}
	else if (iType == 1)
	{
		if (FAILED(m_pShader_Point->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_LIGHT)))
			return E_FAIL;
	}



	return S_OK;
}


CLight* CLight::Create(LIGHT& tLightInfo)
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
	//Safe_Delete(m_pLight);
}
