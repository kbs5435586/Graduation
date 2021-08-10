#include "framework.h"
#include "UI_ButtonNPC.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"
#include "UI_ClassTap.h"

#include "Player.h"
#include "NPC.h"

_int CUI_ButtonNPC::tempNum = 0;

CUI_ButtonNPC::CUI_ButtonNPC()
{
}

CUI_ButtonNPC::CUI_ButtonNPC(const CUI_ButtonNPC& rhs)
{
}

HRESULT CUI_ButtonNPC::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_ButtonNPC::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_iClass = tempNum;
	++tempNum;
	//m_fX = 610.f + ((m_iClass % 5) * 45.f);
	//m_fY = 475.f + ((m_iClass / 5) * 45.f);
	m_fX = (WINCX / 2) - 5 + ((m_iClass % 3) * 55.f);
	m_fY = 615.f + ((m_iClass / 3) * 55.f);


	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	return S_OK;
}

_int CUI_ButtonNPC::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject* uTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 20);
	m_cansee = dynamic_cast<CUI_ClassTap*>(uTemp)->GetBool();

	if (m_cansee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return -1;
		pManagement->AddRef();



		list<CGameObject*> LstTemp = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC");
		npcnumm = LstTemp.size() + 1;

		if (m_iClass < npcnumm)
		{

			if (pManagement->Key_Pressing(KEY_LBUTTON))
			{
				GetCursorPos(&MousePos);
				ScreenToClient(g_hWnd, &MousePos);

				if (MousePos.x > m_fX - (m_fSizeX / 2) && MousePos.x < m_fX + (m_fSizeX / 2))
				{
					if (MousePos.y > m_fY - (m_fSizeY / 2) && MousePos.y < m_fY + (m_fSizeY / 2))
					{
						m_fSizeX = 40.f;
						m_fSizeY = 40.f;
						IsDown = true;
					}
				}
			}


			if (IsDown)
			{
				if (pManagement->Key_Up(KEY_LBUTTON))
				{
					m_fSizeX = 50.f;
					m_fSizeY = 50.f;

					CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 20);
					dynamic_cast<CUI_ClassTap*>(UI)->SetWhich(m_iClass);

					IsDown = false;
				}
			}
		}

		Safe_Release(pManagement);
	}
	
	return _int();
}

_int CUI_ButtonNPC::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_ButtonNPC::Render_GameObject()
{
	if (m_cansee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();


		CGameObject* lTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 20);
		nowNum = dynamic_cast<CUI_ClassTap*>(lTemp)->GetNPCNum();

		_uint now{};
		if (m_iClass == 0)
		{
			CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", m_iClass);
			now = dynamic_cast<CPlayer*>(pTemp)->GetCurMesh();
		}
		else
		{
			if (m_iClass < nowNum + 1)
			{
				CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", m_iClass - 1);
				now = dynamic_cast<CNPC*>(pTemp)->GetCurMesh();
			}
		}

		CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 20);
		int w = dynamic_cast<CUI_ClassTap*>(UI)->GetWhich();


		REP tRep = {};
		if (w == m_iClass)
			tRep.m_arrInt[0] = 1;
		else
			tRep.m_arrInt[0] = 99;

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

		_int iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);


		if (m_iClass < nowNum + 1)
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(now), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();
		m_pBufferCom->Render_VIBuffer();

		Safe_Release(pManagement);
	}
}

HRESULT CUI_ButtonNPC::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}

CUI_ButtonNPC* CUI_ButtonNPC::Create()
{
	CUI_ButtonNPC* pInstance = new CUI_ButtonNPC();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ButtonNPC::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_ButtonNPC* pInstance = new CUI_ButtonNPC();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_ButtonNPC::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}

HRESULT CUI_ButtonNPC::Ready_Component()
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
	if (FAILED(Add_Component(L"Com_InvenShader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Icon");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_IconTexture", m_pTextureCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
