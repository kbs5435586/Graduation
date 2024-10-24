#include "framework.h"
#include "Light.h"
#include "Shader.h"
#include "Transform.h"
#include "Management.h"
#include "Debug_Camera.h"
#include "Terrain_Height.h"


CLight::CLight()
{

}

HRESULT CLight::Ready_Light(LIGHT& tLightInfo)
{
	m_tLight = tLightInfo;

	m_pTransformCom = CTransform::Create();
	m_pShader_Dir = CShader::Create(L"../ShaderFiles/Shader_Deffered_Light.hlsl", "VS_DirLight", "PS_DirLight");
	m_pBufferCom_Dir = CBuffer_RectTex::Create();
	

	if (FAILED(CreateInputLayout(m_tLight.iLightType)))
		return E_FAIL;
	
	return S_OK;
}

void CLight::Update()
{
	if (this->m_tLight.iLightType == 1)
	{
		CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return;

		_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
		m_tLight.vLightPos.y = fY;
	}

}

void CLight::Render()
{
		MAINPASS tMainPass = {};

		_matrix matShadowView = CCamera_Manager::GetInstance()->GetShadowView();
		_matrix matShadowProj = CCamera_Manager::GetInstance()->GetShadowMatProj();
		_matrix matWorld = Matrix_::Identity();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

		REP tRep = {};
		tRep.m_arrMat[0] = matShadowView * matShadowProj;
		tRep.m_arrMat[1] = tMainPass.matViewInv;

		m_pShader_Dir->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
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

		ComPtr<ID3D12DescriptorHeap>	pShadowTex = CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SHADOW)->Get_RTT(0)->pRtt->GetSRV().Get();
		CDevice::GetInstance()->SetTextureToShader(pShadowTex.Get(), TEXTURE_REGISTER::t2);

		ComPtr<ID3D12DescriptorHeap>	pDepthTex = CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(3)->pRtt->GetSRV().Get();
		CDevice::GetInstance()->SetTextureToShader(pDepthTex.Get(), TEXTURE_REGISTER::t3);

		CDevice::GetInstance()->UpdateTable();
		m_pBufferCom_Dir->Render_VIBuffer();
}



HRESULT CLight::CreateInputLayout(_uint iType)
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	//if (iType == 0)
	{
		if (FAILED(m_pShader_Dir->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::NO_DEPTHTEST_NO_WRITE, SHADER_TYPE::SHADER_LIGHT)))
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
	Safe_Release(m_pBufferCom_Dir);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShader_Merge);
	Safe_Release(m_pShader_Dir);
}
