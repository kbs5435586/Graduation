#include "framework.h"
#include "UI_Button.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

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
	
	m_fX = 200.f;
	m_fY = WINCY - 200.f;
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	IsDown = false;
	buttonNum = false;
	buttonnumptr = &buttonNum;
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_INT, &buttonNum);
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_INT_PTR, buttonnumptr);
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


			//++buttonNum;
			//if (buttonNum >= (_uint)CLASS::CLASS_END - 1)
			//	buttonNum = 0;
			buttonNum = true;
			CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INT, &buttonNum);
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

HRESULT CUI_Button::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	//m_pObserverCom = (CObserver*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer");
	//NULL_CHECK_VAL(m_pObserverCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Observer", m_pObserverCom)))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
