#include "framework.h"
#include "UI_Shop.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"
#include "UI_ClassTap.h"

CUI_Shop::CUI_Shop()
{
}

CUI_Shop::CUI_Shop(const CUI_Shop& rhs)
{
}

HRESULT CUI_Shop::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_Shop::Ready_GameObject(void* pArg)
{
	m_IsClone = true;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//m_fX = 900.f;
	//m_fY = 400.f;
	m_fX = (WINCX / 2) + 200;
	m_fY = 400.f;
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	return S_OK;
}

_int CUI_Shop::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject* uTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
	m_cansee = dynamic_cast<CUI_ClassTap*>(uTemp)->GetBool();

	if (m_cansee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return -1;
		pManagement->AddRef();

		CServer_Manager* server = CServer_Manager::GetInstance();
		if (nullptr == server)
			return -1;
		server->AddRef();

		if (pManagement->Key_Down(KEY_LBUTTON))
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

				if (server->Get_NpcSize() < MAX_OWN_NPC)
				{
					CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", server->Get_PlayerID());
					UNIT tPlayerInfo = pTemp->GetPlayerInfo();

					duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
						- server->Get_AddNPC_Cooltime());
					if (cool_time.count() > 0)
					{
						server->send_add_npc_packet();
						server->Set_AddNPC_CoolTime(high_resolution_clock::now());
					}
				}
				IsDown = false;
			}
		}
		Safe_Release(pManagement);
		Safe_Release(server);
		return _int();
	}
}

_int CUI_Shop::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (m_cansee)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
				return E_FAIL;
		}
		
	}

	return _int();
}

void CUI_Shop::Render_GameObject()
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
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();


	Safe_Release(pManagement);
	

}


HRESULT CUI_Shop::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}

CUI_Shop* CUI_Shop::Create()
{
	CUI_Shop* pInstance = new CUI_Shop();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Shop::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_Shop* pInstance = new CUI_Shop();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_Shop::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}

HRESULT CUI_Shop::Ready_Component()
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

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ButtonUI");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	
	Safe_Release(pManagement);

	return S_OK;
}
