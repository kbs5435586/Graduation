#include "stdafx.h"
#include "Tree.h"
#include "Management.h"


CTree::CTree(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTree::CTree(const CTree & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTree::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTree::Ready_GameObject(void * pArg)
{
	TEMP tTemp = {};
	if (nullptr != pArg)
	{
		tTemp = *(TEMP*)pArg;
		if (FAILED(Ready_Component(tTemp.lstrComponentTag)))
			return E_FAIL;
		m_pTransformCom->Set_StateInfo(STATE_POSITION, &tTemp.vPos);
		
		lstrcpy(m_pComponent_Tag, tTemp.lstrComponentTag);
	}

	//m_pTransformCom->Scaling(15.f, 15.f,15.f);
	//m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);
	m_pTransformCom->Set_Add_PosY(0.5f);

	return S_OK;
}

_int CTree::Update_GameObject(const _float & fTimeDelta)
{
	//m_pTransformCom->Scaling(15.f, 15.f, 15.f);
	m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);
	if (m_pPickingCom != nullptr)
		m_pPickingCom->Update_Ray();

	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	if (!m_IsCheckListBox)
	{
		CBuffer_Terrain*	pTerrainBuffer = (CBuffer_Terrain*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return -1;

		_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

		m_pTransformCom->Set_PositionY(fY + m_pTransformCom->Get_Add_PosY());
	}

	Safe_Release(pManagement);



	return _int();
}

_int CTree::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


		if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this)))
			return -1;
	



	return _int();
}

void CTree::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffectHandle();
	if (pEffect == nullptr)
		return;

	pEffect->AddRef();

	_ulong dwNumMaterials = m_pMeshCom->Get_NumMaterials();
	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	for (size_t i = 0; i < dwNumMaterials; i++)
	{
		if (FAILED(SetUp_ConstantTable(pEffect, i)))
			return;

		pEffect->CommitChanges();

		m_pMeshCom->Render_Mesh(i);
	}
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

CTree * CTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTree* pInstance = new CTree(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		return nullptr;
	return pInstance;
}

CGameObject * CTree::Clone_GameObject(void * pArg)
{
	CTree* pInstance = new CTree(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		return nullptr;
	return pInstance;
}

void CTree::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPickingCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	CGameObject::Free();
}

HRESULT CTree::Ready_Component(const _tchar* pComponentTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Mesh");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	m_pMeshCom = (CStatic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, pComponentTag);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CTree::SetUp_ConstantTable(LPD3DXEFFECT pEffect, const _uint & iAttributeID)
{
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix		matView, matProj;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	D3DLIGHT9*	pLightInfo = CLight_Mgr::GetInstance()->Get_LightInfo();
	if (nullptr == pLightInfo)
		return E_FAIL;

	D3DLIGHT9 pLightInfoTemp = *pLightInfo;

	if (m_IsPick)
	{
		pLightInfoTemp.Diffuse.a;
		pLightInfoTemp.Diffuse.r;
		pLightInfoTemp.Diffuse.g = 0.f;
		pLightInfoTemp.Diffuse.b = 0.f;
	}

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfoTemp.Direction, 0.f));
	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfoTemp.Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfoTemp.Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfoTemp.Ambient);

	const SUBSETDESC* pSubSet = m_pMeshCom->Get_SubSetDesc(iAttributeID);
	if (nullptr == pSubSet)
		return E_FAIL;

	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&pSubSet->Material.MatD3D.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&pSubSet->Material.MatD3D.Ambient);
	pEffect->SetFloat("g_fPower", pSubSet->Material.MatD3D.Power);

	pEffect->SetTexture("g_DiffuseTexture", pSubSet->MeshTexture.pDiffuseTexture);

	return S_OK;
}
