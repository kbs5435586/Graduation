#include "framework.h"
#include "UI_End.h"
#include "Management.h"

CUI_End::CUI_End()
	: CUI()
{
}

CUI_End::CUI_End(const CUI_End& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_End::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_End::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fSizeX = 600.f;
	m_fSizeY = 300.f;
	m_fX =WINCX/2.f;
	m_fY = WINCY/2.f - 100.f;


	m_iWinTextureMaxCnt = 13;
	m_iOverTextureMaxCnt = 26;

	return S_OK;
}

_int CUI_End::Update_GameObject(const _float& fTimeDelta)
{

	if (g_IsEnd)
	{
		if (CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx)->GetCurTeam() == TEAM::TEAM_RED)
		{
			if (g_IsWin)
			{
				m_IsWin = true;
			}
			else
			{
				m_IsWin = false;
			}
		}
		else if (CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx)->GetCurTeam() == TEAM::TEAM_BLUE)
		{
			if (g_IsWin)
			{
				m_IsWin = false;
			}
			else
			{
				m_IsWin = true;
			}
		}

		m_fWinTimeDelta += fTimeDelta;
		m_fOverTimeDelta += fTimeDelta;

		if (m_fWinTimeDelta >= 0.1f)
		{
			m_iWinTextureCnt++;
			m_fWinTimeDelta = 0;
		}
		if (m_fOverTimeDelta >= 0.1f)
		{
			m_iOverTextureCnt++;
			m_fOverTimeDelta = 0;
		}

		if (m_iWinTextureCnt >= m_iWinTextureMaxCnt)
			m_iWinTextureCnt = 0;
		if (m_iOverTextureCnt >= m_iOverTextureMaxCnt)
			m_iOverTextureCnt = 0;


	}


	return _int();
}

_int CUI_End::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_IsEnd)
	{
		if (m_pRendererCom != nullptr)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI_BACK, this)))
				return E_FAIL;
		}
	}


	return _int();
}

void CUI_End::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();
	MAINPASS	tMainPass = {};


	_matrix matWorld = Matrix_::Identity();
	_matrix matView = Matrix_::Identity();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;

	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

	if (m_IsWin)
	{
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[0]->GetSRV(m_iWinTextureCnt), TEXTURE_REGISTER::t0);
	}
	else
	{
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1]->GetSRV(m_iOverTextureCnt), TEXTURE_REGISTER::t0);
	}

	CDevice::GetInstance()->UpdateTable();




	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_End::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;


	return S_OK;
}

CUI_End* CUI_End::Create()
{
	CUI_End* pInstance = new CUI_End();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_End::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_End* pInstance = new CUI_End();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_End::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);


	CUI::Free();
}

HRESULT CUI_End::Ready_Component()
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

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_End");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Win");
	NULL_CHECK_VAL(m_pTextureCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture0", m_pTextureCom[0])))
		return E_FAIL;

	m_pTextureCom[1] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_GameOver");
	NULL_CHECK_VAL(m_pTextureCom[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture1", m_pTextureCom[1])))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
