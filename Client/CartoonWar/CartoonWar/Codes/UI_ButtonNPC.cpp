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

HRESULT CUI_ButtonNPC::Ready_GameObject(void* pArg)
{
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_iClass = tempNum;
	++tempNum;
	m_fX = 610.f + ((m_iClass % 5) * 45.f);
	m_fY = 475.f + ((m_iClass / 5) * 45.f);
	
	m_fSizeX = 40.f;
	m_fSizeY = 40.f;

	return S_OK;
}

_int CUI_ButtonNPC::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();


	if (pManagement->Key_Down(KEY_LBUTTON))
	{
		GetCursorPos(&MousePos);
		ScreenToClient(g_hWnd, &MousePos);

		if (MousePos.x > m_fX - (m_fSizeX / 2) && MousePos.x < m_fX + (m_fSizeX / 2))
		{
			if (MousePos.y > m_fY - (m_fSizeY / 2) && MousePos.y < m_fY + (m_fSizeY / 2))
			{
				m_fSizeX = 30.f;
				m_fSizeY = 30.f;
				IsDown = true;
			}
		}
	}


	if (IsDown)
	{
		if (pManagement->Key_Up(KEY_LBUTTON))
		{
			m_fSizeX = 40.f;
			m_fSizeY = 40.f;

			CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 0);
			dynamic_cast<CUI_ClassTap*>(UI)->SetWhich(m_iClass);

			IsDown = false;
		}
	}


	Safe_Release(pManagement);
	return _int();
}

_int CUI_ButtonNPC::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CUI_ButtonNPC::Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint now{};
	if (m_iClass == 0)
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", m_iClass);
		now = dynamic_cast<CPlayer*>(pTemp)->GetCurMesh();
	}
	else
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", m_iClass - 1);
		now = dynamic_cast<CNPC*>(pTemp)->GetCurMesh();
	}

	CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 0);
	int w = dynamic_cast<CUI_ClassTap*>(UI)->GetWhich();
	

	REP tRep = {};
	if(w == m_iClass)
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


	shader->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

	_int iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
		(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

	CDevice::GetInstance()->SetTextureToShader(texture->GetSRV(now), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	buffer->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CUI_ButtonNPC::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	Safe_Release(pManagement);
	return S_OK;
}
