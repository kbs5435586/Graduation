#include "framework.h"
#include "EffectBox.h"
#include "Management.h"

CEffectBox::CEffectBox()
{
}

CEffectBox::CEffectBox(const CEffectBox& rhs)
{
}

HRESULT CEffectBox::Ready_Prototype()
{
	return S_OK;
}

HRESULT CEffectBox::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	CTransform* pTemp = (CTransform*)pArg;

	m_pTransformCom->Set_Matrix(pTemp->Get_Matrix());
	m_pTransformCom->Scaling(10.f, 10.f, 10.f);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));
	
	m_pTransformCom->BackWard(0.5f);
	return S_OK;
}

_int CEffectBox::Update_GameObject(const _float& fTimeDelta)
{
	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY+5.f);

	//m_pTransformCom->BackWard(fTimeDelta);
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > 0.1f)
		return DEAD_OBJ;

	return _int();
}

_int CEffectBox::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom), 10.f)
	{
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		//	return -1;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POST, this)))
			return -1;

	}
	return _int();
}

void CEffectBox::Render_GameObject()
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

	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}

void CEffectBox::Render_PostEffect()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


	m_pShaderCom_PostEffect->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	ComPtr<ID3D12DescriptorHeap>	pPostEffectTex = CManagement::GetInstance()->GetPostEffectTex()->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pPostEffectTex.Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CEffectBox::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;
	if (FAILED(m_pShaderCom_PostEffect->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_POST_EFFECT)))
		return E_FAIL;
	return S_OK;
}

CEffectBox* CEffectBox::Create()
{
	CEffectBox* pInstance = new CEffectBox();
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject* CEffectBox::Clone_GameObject(void* pArg, _uint iIdx)
{
	CEffectBox* pInstance = new CEffectBox();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	m_iLayerIdx = iIdx;

	return pInstance;
}

void CEffectBox::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_PostEffect);
	Safe_Release(m_pFrustumCom);
	CGameObject::Free();
}

HRESULT CEffectBox::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectCol");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Collider");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom_PostEffect = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_PostEffect_Buffer");
	NULL_CHECK_VAL(m_pShaderCom_PostEffect, E_FAIL);
	if (FAILED(Add_Component(L"Com_PostEffectShader", m_pShaderCom_PostEffect)))
		return E_FAIL;

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
