#include "framework.h"
#include "UI_ClassTap.h"
#include "UI_Button.h"
#include "UI_ButtonNPC.h"
#include "UI_CharInterface.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_ClassTap::CUI_ClassTap()
{
}

CUI_ClassTap::CUI_ClassTap(const CUI_ClassTap& rhs)
{
}

HRESULT CUI_ClassTap::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_ClassTap::Ready_GameObject(void* pArg)
{
	m_IsClone = true;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fX = WINCX / 2;
	m_fY = WINCY / 2;

	m_fSizeX = 1000.f;
	m_fSizeY = 500.f;

	which = 0;

	float xxx[9] = { m_fX - 250, m_fX - 380, m_fX - 380, m_fX - 250, m_fX - 250, m_fX - 315, m_fX - 120, m_fX - 120, m_fX - 185 };
	float yyy[9] = { 465, 525, 590, 525, 590, 525, 525, 590, 525 };
	

	return S_OK;
}

_int CUI_ClassTap::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	if (npcnumm < 9)
	{
		list<CGameObject*> LstTemp = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC");	
		npcnumm = LstTemp.size();	
	}
		
	if (pManagement->Key_Up(KEY_I))
		m_cansee = !m_cansee;
	
	if (pManagement->Key_Up(KEY_Q))
	{
		--which;
		if (which < 0)
			which = npcnumm;		
	}
	if (pManagement->Key_Up(KEY_E))
	{
		++which;
		if (which > npcnumm)
			which = 0;
	}

	
	Safe_Release(pManagement);
	return _int();
}

_int CUI_ClassTap::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_ClassTap::Render_GameObject()
{
	if (m_cansee)
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

		//m_pInvenShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		
		_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();
		m_pBufferCom->Render_VIBuffer();
		
		Safe_Release(pManagement);
	}
}

HRESULT CUI_ClassTap::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS)))
		return E_FAIL;
	//if (FAILED(m_pBlendShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::DEFAULT)))
	//	return E_FAIL;
	//if (FAILED(m_pInvenShaderCom->Create_Shader(vecDesc)))
	//	return E_FAIL;

	return S_OK;
}

CUI_ClassTap* CUI_ClassTap::Create()
{
	CUI_ClassTap* pInstance = new CUI_ClassTap();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ClassTap::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_ClassTap* pInstance = new CUI_ClassTap();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_ClassTap::Free()
{
	//CManagement::GetInstance()->UnSubscribe(m_pObserverCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pBlendShaderCom);
	//Safe_Release(m_pInvenShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pButtonTextureCom);
	Safe_Release(m_pNPCTextureCom);
	//Safe_Release(m_pObserverCom);
	Safe_Release(m_pIconTextureCom);
	
	
	CGameObject::Free();
}

HRESULT CUI_ClassTap::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pBlendShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pBlendShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_BlendShader", m_pBlendShaderCom)))
		return E_FAIL;

	m_pInvenShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Tap");
	NULL_CHECK_VAL(m_pInvenShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_InvenShader", m_pInvenShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ClassUI");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pButtonTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ButtonUI");
	NULL_CHECK_VAL(m_pButtonTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ButtonTexture", m_pButtonTextureCom)))
		return E_FAIL;

	m_pNPCTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_RoundButtonUI");
	NULL_CHECK_VAL(m_pNPCTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_NPCTexture", m_pNPCTextureCom)))
		return E_FAIL;

	m_pIconTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Icon");
	NULL_CHECK_VAL(m_pIconTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_IconTexture", m_pIconTextureCom)))
		return E_FAIL;

	m_pPressedNPCTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_RoundButtonUI_Pressed");
	NULL_CHECK_VAL(m_pPressedNPCTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_PressedNPCTexture", m_pPressedNPCTextureCom)))
		return E_FAIL;


	//m_pObserverCom = (CObserver*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer");
	//NULL_CHECK_VAL(m_pObserverCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Observer", m_pObserverCom)))
	//	return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}
