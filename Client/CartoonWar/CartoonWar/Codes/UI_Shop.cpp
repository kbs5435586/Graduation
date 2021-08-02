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


			if (server->Get_NpcSize() < 14)
			{
				duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
					- server->Get_AddNPC_Cooltime());
				if (cool_time.count() > 2)
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
