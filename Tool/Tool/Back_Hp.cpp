#include "stdafx.h"
#include "Back_Hp.h"
#include "Management.h"
#include "MainFrm.h"
#include "SecondWnd.h"
#include "UI.h"


CBack_Hp::CBack_Hp(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBack_Hp::CBack_Hp(const CBack_Hp & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBack_Hp::Ready_Prototype()
{
	return S_OK;
}

HRESULT CBack_Hp::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		m_pGameObject_Tag = (_tchar*)pArg;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_fX = 400.f;
	m_fY = 300.f;

	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	m_pTransformCom->SetUp_Speed(5.0f, D3DXToRadian(90.0f));
	m_IsUI = TRUE;
	m_eUI = UI_HP_BACK;

	return S_OK;
}

_int CBack_Hp::Update_GameObject(const _float & fTimeDelta)
{
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (nullptr == pMainFrm)
		return _int();
	CSecondWnd* pSecond = dynamic_cast<CSecondWnd*>(pMainFrm->m_MainSplt.GetPane(0, 1));

	Move_UI_Window(pSecond);


	return _int();
}

_int CBack_Hp::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_UI, this)))
		return -1;



	return _int();
}

void CBack_Hp::Render_GameObject()
{
	if (nullptr == m_pBufferCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffectHandle();
	if (nullptr == pEffect)
		return;


	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(2);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

CBack_Hp * CBack_Hp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBack_Hp*	pInstance = new CBack_Hp(pGraphic_Device);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CUI Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBack_Hp::Clone_GameObject(void * pArg)
{
	CBack_Hp*	pInstance = new CBack_Hp(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CUI Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBack_Hp::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	CGameObject::Free();
}

HRESULT CBack_Hp::Ready_Component()
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

	// For.Com_Texture
	m_pTextureCom = (CTexture*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Texture_HP_Back", L"Component_Texture_HP_Back");
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	// For.Com_Buffer
	m_pBufferCom = (CBuffer_RcTex*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	// For.Com_Shader
	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Default");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CBack_Hp::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	_matrix			matWorld;
	D3DXMatrixIdentity(&matWorld);

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;

	matWorld._41 = m_fX - (g_iBackCX >> 1);
	matWorld._42 = -m_fY + (g_iBackCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);


	_matrix			matView;
	D3DXMatrixIdentity(&matView);
	pEffect->SetMatrix("g_matView", &matView);

	_matrix			matProj;
	D3DXMatrixOrthoLH(&matProj, g_iBackCX, g_iBackCY, 0.0f, 1.f);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture");

	return S_OK;
}

void CBack_Hp::Move_UI_Window(CSecondWnd* pSecond)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	if (m_IsPick)
	{
		if (pSecond->m_pUI->m_IsMove)
		{
			if (pSecond->m_pUI->m_IsMove_Side)
			{
				m_fX += pSecond->m_pUI->m_fSideCnt;
				pSecond->m_pUI->m_IsMove_Side = false;
			}
			else if (pSecond->m_pUI->m_IsMove_Cross)
			{
				m_fY += pSecond->m_pUI->m_fCrossCnt;
				pSecond->m_pUI->m_IsMove_Cross = false;
			}
			pSecond->m_pUI->m_IsMove = false;
		}
	}

	Safe_Release(pManagement);

}

