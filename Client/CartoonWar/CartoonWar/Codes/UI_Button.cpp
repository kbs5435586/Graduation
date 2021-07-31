#include "framework.h"
#include "UI_Button.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

_int CUI_Button::tempNum = 0;

CUI_Button::CUI_Button()
{
}

CUI_Button::CUI_Button(const CUI_Button& rhs)
{
}

HRESULT CUI_Button::Ready_GameObject(void* pArg)
{
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_iClass = tempNum;
	++tempNum;

	m_fX = 300.f + ((m_iClass % 3) * 100.f);
	m_fY = 300 + ((m_iClass / 3) * 100.f);
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	lstTemp = CManagement::GetInstance()->Get_List(DATA_TYPE::DATA_NPC);
	//CManagement::GetInstance()->Notify(DATA_TYPE::DATA_NPC,)
	return S_OK;
}

_int CUI_Button::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
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

			_int whichnum = m_pObserverCom->GetWhichInfo();
			auto temp = (int*)m_pObserverCom->GetNPC(whichnum);


			if (*temp == 0)
			{
				if (m_iClass == 2 || m_iClass == 4 || m_iClass == 7)
				{}
				else
					*temp = m_iClass;
			}
			else if (*temp == 1 || *temp == 2)
			{
				if (m_iClass == 0 || m_iClass == 1 || m_iClass == 2)
					*temp = m_iClass;
			}
			else if (*temp == 3 || *temp == 4)
			{
				if (m_iClass == 0 || m_iClass == 3 || m_iClass == 4)
					*temp = m_iClass;
			}
			else if (*temp == 5)
			{
				if (m_iClass == 0 || m_iClass == 5)
					*temp = m_iClass;
			}
			else if (*temp == 6 || *temp == 7)
			{
				if (m_iClass == 0 || m_iClass == 6 || m_iClass == 7)
					*temp = m_iClass;
			}
			else if (*temp == 8)
			{
				if (m_iClass == 0 || m_iClass == 8)
					*temp = m_iClass;
			}
	
			m_pObserverCom->ReUpdate_Observer(DATA_TYPE::DATA_NPC);

			IsDown = false;
		}	
	}
	

	Safe_Release(pManagement);
	return _int();
}

_int CUI_Button::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CUI_Button::Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture)
{

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();



	REP tRep = {};
	_int whichnum = m_pObserverCom->GetWhichInfo();
	auto temp = (int*)m_pObserverCom->GetNPC(whichnum);
	
	
	if (*temp == 0)
	{
		if (m_iClass == 2 || m_iClass == 4 || m_iClass == 7)
			tRep.m_arrInt[0] = 2;
		else
			tRep.m_arrInt[0] = 0;
	}
	else if(*temp == 1 || *temp == 2)
	{
		if (m_iClass == 0 || m_iClass == 1 || m_iClass == 2)
			tRep.m_arrInt[0] = 0;
		else
			tRep.m_arrInt[0] = 2;
	}
	else if (*temp == 3 || *temp == 4)
	{
		if (m_iClass == 0 || m_iClass == 3 || m_iClass == 4)
			tRep.m_arrInt[0] = 0;
		else
			tRep.m_arrInt[0] = 2;
	}
	else if (*temp == 5)
	{
		if (m_iClass == 0 || m_iClass == 5)
			tRep.m_arrInt[0] = 0;
		else
			tRep.m_arrInt[0] = 2;
	}
	else if (*temp == 6 || *temp == 7)
	{
		if (m_iClass == 0 || m_iClass == 6 || m_iClass == 7)
			tRep.m_arrInt[0] = 0;
		else
			tRep.m_arrInt[0] = 2;
	}
	else if (*temp == 8)
	{
		if (m_iClass == 0 || m_iClass == 8)
			tRep.m_arrInt[0] = 0;
		else
			tRep.m_arrInt[0] = 2;
	}

	if(*temp == m_iClass)
		tRep.m_arrInt[0] = 1;

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

	
	CDevice::GetInstance()->SetTextureToShader(texture->GetSRV(m_iClass), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	buffer->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CUI_Button::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	Safe_Release(pManagement);
	return S_OK;
}

