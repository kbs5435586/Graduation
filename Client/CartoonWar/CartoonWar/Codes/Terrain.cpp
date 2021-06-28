#include "framework.h"
#include "Terrain.h"
#include "Management.h"

CTerrain::CTerrain()
	: CGameObject()
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	//m_pTransformCom->SetUp_RotationY(XMConvertToRadians(90.f) );
	//m_pTransformCom->Scaling(_vec3(1000.f, 1000.f, 1000.f));
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));
	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CTerrain::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	return _int();
}

void CTerrain::Render_GameObject()
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

	FOG tFog = { 0.f, 5.f };



	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b6)->SetData((void*)&tFog);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b6)->GetCBV().Get(), iOffeset, CONST_REGISTER::b6);


	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);

	//ComPtr<ID3D12DescriptorHeap>	pTextureDesc = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(4)->pRtt->GetSRV().Get();
	//CDevice::GetInstance()->SetTextureToShader(pTextureDesc.Get(), TEXTURE_REGISTER::t1);

	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	//m_pNaviCom->Render_Navigation();


	Safe_Release(pManagement);;
}

void CTerrain::Render_GameObject_Shadow()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);


	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();

	m_pBufferCom->Render_VIBuffer();
	//m_pNaviCom->Render_Navigation();
	Safe_Release(pManagement);;

}

HRESULT CTerrain::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}


CTerrain* CTerrain::Create()
{
	CTerrain* pInstance = new CTerrain();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTerrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTerrain::Clone_GameObject(void* pArg, _uint iIdx)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTerrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFrustumCom);

	CGameObject::Free();
}

HRESULT CTerrain::Ready_Component()
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

	m_pBufferCom = (CBuffer_Terrain*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
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
