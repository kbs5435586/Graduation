#include "stdafx.h"
#include "Player.h"
#include "Management.h"
#include "OnHeadFont.h"
#include "Light_Mgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Ready_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg)
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

	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);

	//m_pMeshCom->Set_AnimationSet(25);

	m_pTransformCom->SetUp_Speed(5.f, D3DXToRadian(90.0f));
	m_iNewAnimationNum = 0;
	return S_OK;
}

_int CPlayer::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pMeshCom)
		return _int();
	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();


// 	if (pManagement->KeyDown(KEY_SPACE))
// 	{
// 		m_IsOnce = true;
// 		m_eCurState = CPlayer::STATE_JUMP;
// 	}
// 	else if (pManagement->KeyDown(KEY_SHIFT))
// 	{
// 		m_IsOnce = false;
// 		m_eCurState = CPlayer::STATE_IDLE;
// 	}


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

_int CPlayer::LastUpdate_GameObject(const _float & fTimeDelta)
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
		//Jump
		{
			if (m_eCurState == CPlayer::STATE_JUMP)
				m_eCurState = CPlayer::STATE_JUMP_END;
			if (m_eCurState == CPlayer::STATE_JUMP_END)
				m_fOnIdleTime += fTimeDelta;
			if (m_fOnIdleTime >= 0.1f)
			{
				m_eCurState = CPlayer::STATE_IDLE;
				m_IsOnce = false;
				m_fOnIdleTime = 0.f;
			}
		}

		m_fMove = 0.f;
	}
	else
	{
		m_fMove = 1.f;
	}

	return _int();
}

void CPlayer::Render_GameObject()
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

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer*	pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pOnHeadFont);
	Safe_Release(m_pPickingCom);
	CGameObject::Free();
}

HRESULT CPlayer::Ready_Component()
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
	m_pMeshCom = (CDynamic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Mesh_Popori");
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	return NOERROR;
}


HRESULT CPlayer::SetUp_ConstantTable(LPD3DXEFFECT pEffect, const D3DXMESHCONTAINER_DERIVED* pMeshContainer, const _uint& iAttributeID)
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

	return NOERROR;
}

void CPlayer::Change_State(STATE eID)
{
	if (m_ePreState != eID)
	{
		switch (eID)
		{
		case CPlayer::STATE_IDLE:
			m_pMeshCom->Set_AnimationSet((_int)CPlayer::STATE_IDLE);
			break;
		case CPlayer::STATE_JUMP:
			m_pMeshCom->Set_AnimationSet((_int)CPlayer::STATE_JUMP);
			break;
		case CPlayer::STATE_DEAD:
			break;
		case CPlayer::STATE_JUMP_END:
			m_pMeshCom->Set_AnimationSet((_int)CPlayer::STATE_JUMP_END);
			break;
		}
		if (m_eCurState == m_pMeshCom->Get_CurrentState())
			m_ePreState = eID;
	}
}
