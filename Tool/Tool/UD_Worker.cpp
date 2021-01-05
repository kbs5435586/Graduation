#include "stdafx.h"
#include "UD_Worker.h"
#include "Management.h"
#include "OnHeadFont.h"

CUD_Worker::CUD_Worker(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUD_Worker::CUD_Worker(const CUD_Worker & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUD_Worker::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUD_Worker::Ready_GameObject(void * pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_vec3 vPos = *(_vec3*)pArg;
		m_pTransformCom->Set_StateInfo(STATE_POSITION, &vPos);

	}
	CComponent* pMesh = this->Get_ComponentPointer(L"Com_Mesh");
	pMesh->Get_IsDyanmic() = true;

	m_pOnHeadFont = COnHeadFont::Create(m_pGraphic_Device);
	m_pOnHeadFont->AddRef();

	m_pTransformCom->Scaling(0.05f, 0.05f, 0.05f);
	m_pTransformCom->SetUp_Speed(5.f, D3DXToRadian(90.0f));
	m_pMeshCom->Set_AnimationSet(1);
	return S_OK;
}

_int CUD_Worker::Update_GameObject(const _float & fTimeDelta)
{
	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	if (pManagement->KeyDown(KEY_SPACE))
	{
		m_IsOnce = true;
		m_eCurState = CUD_Worker::STATE_SPON1;
	}

	Change_State(m_eCurState);
	m_pMeshCom->Play_Animation(fTimeDelta*m_fMove);

	CBuffer_Terrain*	pTerrainBuffer = (CBuffer_Terrain*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY);

	Safe_Release(pManagement);

	return _int();
}

_int CUD_Worker::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (nullptr == m_pMeshCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_NONEALPHA, this)))
		return -1;

	m_pOnHeadFont->SetPos(*m_pTransformCom->Get_StateInfo(STATE_POSITION));
	if (m_IsPick)
		m_pOnHeadFont->LastUpdate_GameObject(fTimeDelta, m_IsPick);


	if (m_pMeshCom->Get_EndLoop() && m_IsOnce)
	{
		//SPON
		{
			if (m_eCurState == CUD_Worker::STATE_SPON1)
				m_eCurState = CUD_Worker::STATE_SPON2;
			if (m_eCurState == CUD_Worker::STATE_SPON2)
				m_fOnIdleTime += fTimeDelta;
			if (m_fOnIdleTime >= 0.05f)
			{
				m_eCurState = CUD_Worker::STATE_IDLE;
				m_IsOnce = false;
				m_fOnIdleTime = 0.f;
			}
		}

		{

		}
		m_fMove = 0.f;
	}
	else
	{
		m_fMove = 1.f;
	}

	return _int();
}

void CUD_Worker::Render_GameObject()
{
	if (nullptr == m_pMeshCom ||
		nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT		pEffect = m_pShaderCom->GetEffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	_ulong dwNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	for (size_t i = 0; i < dwNumMeshContainer; i++)
	{
		m_pMeshCom->Update_Skinning(i);

		const D3DXMESHCONTAINER_DERIVED* pMeshContainer = m_pMeshCom->Get_MeshContainer(i);

		for (size_t j = 0; j < pMeshContainer->NumMaterials; ++j)
		{
			if (FAILED(SetUp_ConstantTable(pEffect, pMeshContainer, j)))
				return;

			pEffect->CommitChanges();

			m_pMeshCom->Render_Mesh(i, j);
		}

	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

CUD_Worker * CUD_Worker::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUD_Worker*	pInstance = new CUD_Worker(pGraphic_Device);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUD_Worker::Clone_GameObject(void * pArg)
{
	CUD_Worker*	pInstance = new CUD_Worker(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUD_Worker::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pOnHeadFont);
	Safe_Release(m_pPickingCom);
	CGameObject::Free();
}

void CUD_Worker::Change_State(STATE eID)
{
	if (m_ePreState != eID)
	{
		switch (eID)
		{
		case CUD_Worker::STATE_SPON1:
			m_pMeshCom->Set_AnimationSet((_int)CUD_Worker::STATE_SPON1);
			break;
		case CUD_Worker::STATE_SPON2:
			m_pMeshCom->Set_AnimationSet((_int)CUD_Worker::STATE_SPON2);
			break;
		case CUD_Worker::STATE_IDLE:
			m_pMeshCom->Set_AnimationSet((_int)CUD_Worker::STATE_IDLE);
			break;
		}
		if (m_eCurState == m_pMeshCom->Get_CurrentState())
			m_ePreState = eID;
	}
}

HRESULT CUD_Worker::Ready_Component()
{
	CManagement*		pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	// For.Com_Transform
	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Mesh");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	// For.Com_Mesh
	m_pMeshCom = (CDynamic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Mesh_UD_Worker");
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUD_Worker::SetUp_ConstantTable(LPD3DXEFFECT pEffect, const D3DXMESHCONTAINER_DERIVED * pMeshContainer, const _uint & iAttributeID)
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


	const SUBSETDESC* pSubSet = &pMeshContainer->pSubSetDesc[iAttributeID];
	if (nullptr == pSubSet)
		return E_FAIL;

	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&pSubSet->Material.MatD3D.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&pSubSet->Material.MatD3D.Ambient);
	pEffect->SetFloat("g_fPower", pSubSet->Material.MatD3D.Power);

	pEffect->SetTexture("g_DiffuseTexture", pSubSet->MeshTexture.pDiffuseTexture);

	return S_OK;
}
