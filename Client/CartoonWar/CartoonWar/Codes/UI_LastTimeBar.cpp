#include "framework.h"
#include "UI_LastTimeBar.h"
#include "Management.h"

CUI_LastTimeBar::CUI_LastTimeBar()
	: CUI()
{
}

CUI_LastTimeBar::CUI_LastTimeBar(const CUI_LastTimeBar& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_LastTimeBar::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_LastTimeBar::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fSizeX = 300.f;
	m_fSizeY = 15.f;
	m_fX = WINCX/2.f;
	m_fY = WINCY/ 2.f - m_fSizeY*29.5f;


	return S_OK;
}

_int CUI_LastTimeBar::Update_GameObject(const _float& fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= 1.f)
	{
		g_iTotalTime += 1;
		m_iTimeCnt += 1;
		m_fTimeDelta = 0.f;
	}
	return _int();
}

_int CUI_LastTimeBar::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_LastTimeBar::Render_GameObject()
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

	REP tRep = {};
	tRep.m_arrInt[0] = m_iTimeCnt;

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(3), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();



	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_LastTimeBar::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;


	return S_OK;
}

CUI_LastTimeBar* CUI_LastTimeBar::Create()
{
	CUI_LastTimeBar* pInstance = new CUI_LastTimeBar();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_LastTimeBar::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_LastTimeBar* pInstance = new CUI_LastTimeBar();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_LastTimeBar::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


	CUI::Free();
}

HRESULT CUI_LastTimeBar::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_LastTime");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Gold");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}