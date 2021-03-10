#include "framework.h"
#include "Terrain_Height.h"
#include "Management.h"

CTerrain_Height::CTerrain_Height()
	: CGameObject()
{
}

CTerrain_Height::CTerrain_Height(const CTerrain_Height& rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain_Height::Ready_Prototype()
{

	return S_OK;
}

HRESULT CTerrain_Height::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	m_pTransformCom->Scaling(_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));
	return S_OK;
}

_int CTerrain_Height::Update_GameObject(const _float& fTimeDelta)
{

	//m_pBufferCom->Culling_Frustum(m_pFrustumCom, m_pTransformCom->Get_Matrix());
	return _int();
}

_int CTerrain_Height::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	return _int();
}

void CTerrain_Height::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	FOG tFog = {0.f, 5.f};



	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b6)->SetData((void*)&tFog);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b6)->GetCBV().Get(), iOffeset, CONST_REGISTER::b6);


	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom,  TEXTURE_REGISTER::t0);

	ComPtr<ID3D12DescriptorHeap>	pTextureDesc = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(4)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc.Get(), TEXTURE_REGISTER::t1);
	
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	m_pNaviCom->Render_Navigation();


	Safe_Release(pManagement);
}

HRESULT CTerrain_Height::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;

	return S_OK;
}

CTerrain_Height* CTerrain_Height::Create()
{
	CTerrain_Height* pInstance = new CTerrain_Height();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTerrain_Height Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTerrain_Height::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	CTerrain_Height* pInstance = new CTerrain_Height(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTerrain_Height Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CTerrain_Height::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFrustumCom);
	
	CGameObject::Free();
}

HRESULT CTerrain_Height::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_Terrain_Height*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;
		
	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
