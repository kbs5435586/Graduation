#include "framework.h"
#include "UI_ButtonNPC.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"


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
	m_fX = 760.f + ((m_iClass % 5) * 45.f);
	m_fY = 475.f + ((m_iClass / 5) * 45.f);
	
	m_fSizeX = 40.f;
	m_fSizeY = 40.f;

	lstTemp = CManagement::GetInstance()->Get_List(DATA_TYPE::DATA_NPC);
	//CManagement::GetInstance()->Notify(DATA_TYPE::DATA_NPC,)
	return S_OK;
}

_int CUI_ButtonNPC::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	

	//*m_Active = m_pObserverCom->GetTapInfo();
	if (*m_Active)
	{
		m_fX += 50.f;
		*m_Active = !*m_Active;
		//CManagement::GetInstance()->Notify(DATA_TYPE::DATA_TAP, which);
	}

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

			*which = m_iClass;
			CManagement::GetInstance()->Notify(DATA_TYPE::DATA_WHICH, which);

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

HRESULT CUI_ButtonNPC::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	Safe_Release(pManagement);
	return S_OK;
}
