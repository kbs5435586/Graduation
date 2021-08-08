#include "framework.h"
#include "UI_Shop.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_Shop::CUI_Shop()
{
}

CUI_Shop::CUI_Shop(const CUI_Shop& rhs)
{
}

HRESULT CUI_Shop::Ready_GameObject(void* pArg)
{
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

_int CUI_Shop::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
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

			list<CGameObject*> LstTemp = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC");
			npcNum = LstTemp.size();
			

			if (npcNum < 14)
			{
				UNIT tPlayerInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_RED };

				if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_NPC", (_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", nullptr, (void*)&tPlayerInfo)))
					return E_FAIL;
			}
		

			IsDown = false;
		}
	}


	Safe_Release(pManagement);
	return _int();
}

_int CUI_Shop::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CUI_Shop::Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture)
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


	shader->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	CDevice::GetInstance()->SetTextureToShader(texture->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	buffer->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CUI_Shop::Ready_Component()
{
	return E_NOTIMPL;
}
