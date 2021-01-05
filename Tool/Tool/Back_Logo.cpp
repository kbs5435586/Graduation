#include "stdafx.h"
#include "..\Headers\Back_Logo.h"
#include "Management.h"



CBack_Logo::CBack_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBack_Logo::CBack_Logo(const CBack_Logo & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBack_Logo::Ready_Prototype()
{
	return S_OK;
}

HRESULT CBack_Logo::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Speed(5.0f, D3DXToRadian(90.0f));

	m_pTransformCom->Scaling(3.f, 2.f, 1.f);
	m_pTransformCom->Set_StateInfo(STATE_POSITION, &_vec3(0.0f, 0.f, 0.f));

	return S_OK; 
}

_int CBack_Logo::Update_GameObject(const _float & fTimeDelta)
{
	static _float		fTmp = 1.f;


	return _int();
}

_int CBack_Logo::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_NONEALPHA, this)))
		return -1;

	return _int();
}

void CBack_Logo::Render_GameObject()
{
	if (nullptr == m_pBufferCom||nullptr==m_pShaderCom)
		return;

	m_pTransformCom->SetUp_OnGraphicDev();


	if (FAILED(m_pTextureCom->SetUp_OnGraphicDev()))
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	_matrix		matTmp;
	D3DXMatrixIdentity(&matTmp);
	pEffect->SetMatrix("g_matWorld", &m_pTransformCom->Get_Matrix());
	pEffect->SetMatrix("g_matView", &matTmp);
	pEffect->SetMatrix("g_matProj", &matTmp);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture")))
		return;

	_uint		iNumPass = 0;

	pEffect->Begin(&iNumPass, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CBack_Logo::Ready_Component()
{
	CManagement*		pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;
	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;
	m_pTextureCom = (CTexture*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Texture_Logo");
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	m_pBufferCom = (CBuffer_RcTex*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;
	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Default");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

CBack_Logo * CBack_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBack_Logo*	pInstance = new CBack_Logo(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CBack_Logo Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBack_Logo::Clone_GameObject()
{
	CBack_Logo*	pInstance = new CBack_Logo(*this);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CBack_Logo Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBack_Logo::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	CGameObject::Free();
}
