#include "framework.h"
#include "UI_Button.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"
#include "UI_ClassTap.h"

#include "Player.h"
#include "NPC.h"

_int CUI_Button::tempNum = 0;

CUI_Button::CUI_Button()
{
}

CUI_Button::CUI_Button(const CUI_Button& rhs)
{
}

HRESULT CUI_Button::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_Button::Ready_GameObject(void* pArg)
{
	if (pArg)
	{
		vTemp = *(_vec2*)pArg;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_ButtonClass = CLASS(tempNum);
	++tempNum;

	//m_fX = 300.f + ((m_iClass % 3) * 100.f);
	//m_fY = 300 + ((m_iClass / 3) * 100.f);

	m_fX = vTemp.x;
	m_fY = vTemp.y;

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	return S_OK;
}

_int CUI_Button::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject* uTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
	m_cansee = dynamic_cast<CUI_ClassTap*>(uTemp)->GetBool();

	if (m_cansee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return -1;
		pManagement->AddRef();


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


				CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
				_int whichnum = dynamic_cast<CUI_ClassTap*>(UI)->GetWhich();

				CLASS cTemp{};

				if (whichnum == 0)
				{
					CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
					//m_iCurMeshNum = dynamic_cast<CPlayer*>(pTemp)->GetCurMesh();
					cTemp = pTemp->GetClass();
				}
				else
				{
					CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
					//m_iCurMeshNum = dynamic_cast<CNPC*>(pTemp)->GetCurMesh();
					cTemp = pTemp->GetClass();
				}


				if (cTemp == CLASS::CLASS_WORKER)
				{
					if (m_ButtonClass == CLASS(2) || m_ButtonClass == CLASS(4) || m_ButtonClass == CLASS::CLASS_MMAGE)
					{
					}
					else
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);				
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}
				else if (cTemp == CLASS::CLASS_CAVALRY || cTemp == CLASS(2))
				{
					if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_CAVALRY || m_ButtonClass == CLASS(2))
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}
				else if (cTemp == CLASS::CLASS_INFANTRY || cTemp == CLASS(4))
				{
					if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_INFANTRY || m_ButtonClass == CLASS(4))
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}
				else if (cTemp == CLASS::CLASS_SPEARMAN)
				{
					if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_SPEARMAN)
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}
				else if (cTemp == CLASS::CLASS_MAGE || cTemp == CLASS::CLASS_MMAGE)
				{
					if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_MAGE || m_ButtonClass == CLASS::CLASS_MMAGE)
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}
				else if (cTemp == CLASS::CLASS_ARCHER)
				{
					if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_ARCHER)
					{
						if (whichnum == 0)
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
							//dynamic_cast<CPlayer*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
						else
						{
							CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
							//dynamic_cast<CNPC*>(pTemp)->SetCurMesh(m_iClass);
							pTemp->SetClass(CLASS(m_ButtonClass));
						}
					}
				}

				IsDown = false;
			}
		}

		Safe_Release(pManagement);
	}

	return _int();
}

_int CUI_Button::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_Button::Render_GameObject()
{
	if (m_cansee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();


		REP tRep = {};

		CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
		_int whichnum = dynamic_cast<CUI_ClassTap*>(UI)->GetWhich();

		CLASS cTemp{};

		if (whichnum == 0)
		{
			CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", whichnum);
			//m_iCurMeshNum = dynamic_cast<CPlayer*>(pTemp)->GetCurMesh();
			cTemp = pTemp->GetClass();
		}
		else
		{
			CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", whichnum - 1);
			//m_iCurMeshNum = dynamic_cast<CNPC*>(pTemp)->GetCurMesh();
			cTemp = pTemp->GetClass();
		}


		if (cTemp == CLASS::CLASS_WORKER)
		{
			if (m_ButtonClass == CLASS(2) || m_ButtonClass == CLASS(4) || m_ButtonClass == CLASS::CLASS_MMAGE)
				tRep.m_arrInt[0] = 2;
			else
				tRep.m_arrInt[0] = 0;
		}
		else if (cTemp == CLASS::CLASS_CAVALRY || cTemp == CLASS(2))
		{
			if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_CAVALRY || m_ButtonClass == CLASS(2))
				tRep.m_arrInt[0] = 0;
			else
				tRep.m_arrInt[0] = 2;
		}
		else if (cTemp == CLASS::CLASS_INFANTRY || cTemp == CLASS(4))
		{
			if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_INFANTRY || m_ButtonClass == CLASS(4))
				tRep.m_arrInt[0] = 0;
			else
				tRep.m_arrInt[0] = 2;
		}
		else if (cTemp == CLASS::CLASS_SPEARMAN)
		{
			if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_SPEARMAN)
				tRep.m_arrInt[0] = 0;
			else
				tRep.m_arrInt[0] = 2;
		}
		else if (cTemp == CLASS::CLASS_MAGE || cTemp == CLASS::CLASS_MMAGE)
		{
			if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_MAGE || m_ButtonClass == CLASS::CLASS_MMAGE)
				tRep.m_arrInt[0] = 0;
			else
				tRep.m_arrInt[0] = 2;
		}
		else if (cTemp == CLASS::CLASS_ARCHER)
		{
			if (m_ButtonClass == CLASS::CLASS_WORKER || m_ButtonClass == CLASS::CLASS_ARCHER)
				tRep.m_arrInt[0] = 0;
			else
				tRep.m_arrInt[0] = 2;
		}

		if (cTemp == CLASS(m_ButtonClass))
			tRep.m_arrInt[0] = 1;

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


		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(_uint(m_ButtonClass)), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();
		m_pBufferCom->Render_VIBuffer();

		Safe_Release(pManagement);
	}
}

HRESULT CUI_Button::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}

CUI_Button* CUI_Button::Create()
{
	CUI_Button* pInstance = new CUI_Button();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Button::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_Button* pInstance = new CUI_Button();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_Button::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}

HRESULT CUI_Button::Ready_Component()
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

void CUI_Button::setObs(CObserver* _obs)
{
	//m_pObserverCom = _obs;
	//m_pObserverCom->AddRef();
}

