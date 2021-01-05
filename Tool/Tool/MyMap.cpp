#include "stdafx.h"
#include "MyMap.h"
#include "Management.h"
#include "OnHeadFont.h"

CMyMap::CMyMap(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMyMap::CMyMap(const CMyMap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMyMap::Ready_Prototype()
{
	return S_OK;
}

HRESULT CMyMap::Ready_GameObject(void * pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pOnHeadFont = COnHeadFont::Create(m_pGraphic_Device);
	m_pOnHeadFont->AddRef();

	if (pArg != nullptr)
		m_pTransformCom->Set_StateInfo(STATE_POSITION, &*(_vec3*)pArg);

	m_pTransformCom->Scaling(0.01f, 0.01f, 0.01f);
	return S_OK;
}

_int CMyMap::Update_GameObject(const _float & fTimeDelta)
{
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

		m_pTransformCom->Set_PositionY(fY);
	}

	Safe_Release(pManagement);

	return _int();
}

_int CMyMap::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_NONEALPHA, this)))
		return -1;

	m_pOnHeadFont->SetPos(*m_pTransformCom->Get_StateInfo(STATE_POSITION));
	if (m_IsPick == true)
		m_pOnHeadFont->LastUpdate_GameObject(fTimeDelta, m_IsPick);
	return _int();
}

void CMyMap::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffectHandle();
	if (pEffect == nullptr)
		return;

	pEffect->AddRef();

	_ulong dwNumMaterials = m_pMeshCom->Get_NumMaterials();

	static _uint iPassIdx = 0;
	if (IsWireFrame)
		iPassIdx = 1;
	else
		iPassIdx = 0;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(iPassIdx);
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

CMyMap * CMyMap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMyMap* pInstance = new CMyMap(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		return nullptr;
	return pInstance;
}

CGameObject * CMyMap::Clone_GameObject(void * pArg)
{
	CMyMap* pInstance = new CMyMap(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		return nullptr;
	return pInstance;
}

void CMyMap::Free()
{
	Safe_Release(m_pOnHeadFont);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPickingCom);

	CGameObject::Free();
}

HRESULT CMyMap::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pMeshCom = (CStatic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Mesh_Map");
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Mesh");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CMyMap::SetUp_ConstantTable(LPD3DXEFFECT pEffect, const _uint & iAttributeID)
{
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix		matView, matProj;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9*	pLightInfo = CLight_Mgr::GetInstance()->Get_LightInfo();
	if (nullptr == pLightInfo)
		return E_FAIL;
	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	const SUBSETDESC* pSubSet = m_pMeshCom->Get_SubSetDesc(iAttributeID);
	if (nullptr == pSubSet)
		return E_FAIL;

	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&pSubSet->Material.MatD3D.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&pSubSet->Material.MatD3D.Ambient);
	pEffect->SetFloat("g_fPower", pSubSet->Material.MatD3D.Power);

	pEffect->SetTexture("g_DiffuseTexture", pSubSet->MeshTexture.pDiffuseTexture);

	return S_OK;
}
