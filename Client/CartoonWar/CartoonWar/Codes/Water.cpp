#include"framework.h"
#include "Water.h"
#include "Management.h"

CWater::CWater()
	: CGameObject()
{
}

CWater::CWater(const CWater& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWater::Ready_Prototype()
{
	return S_OK;
}

HRESULT CWater::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	//_vec3 vPos = { 100.f , 150.f, 100.f };
	_vec3 vPos = { 3079.56f, 45.f, 1106.38f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->Scaling(580.f, 580.f, 2.f);
	m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	return S_OK;

}

_int CWater::Update_GameObject(const _float& fTimeDelta)
{
	m_tTexInfo.fFrameTime += fTimeDelta*0.1f;
	if (m_tTexInfo.fFrameTime > 1.f)
	{
		m_tTexInfo.fFrameTime = -1.f;
	}
	return _int();
}

_int CWater::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
	}

	return _int();
}

void CWater::Render_GameObject()
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



	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->SetData((void*)&m_tTexInfo);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->GetCBV().Get(), iOffeset, CONST_REGISTER::b4);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(1), TEXTURE_REGISTER::t0);
	//CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(1), TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	Safe_Release(pManagement);
}

void CWater::Render_PostEffect()
{
}

HRESULT CWater::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED, BLEND_TYPE::DEFAULT, D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST)))
		return E_FAIL;

	return S_OK;
}

CWater* CWater::Create()
{
	CWater* pInstance = new CWater();
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject* CWater::Clone_GameObject(void* pArg, _uint iIdx)
{
	CWater* pInstance = new CWater();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	m_iLayerIdx = iIdx;

	return pInstance;
}

void CWater::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pFrustumCom);
	CGameObject::Free();
}

HRESULT CWater::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain_Tess");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	//m_pTextureCom
	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Water");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}
