#include "framework.h"
#include "UI_CharInterface.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_CharInterface::CUI_CharInterface()
{
}

CUI_CharInterface::CUI_CharInterface(const CUI_CharInterface& rhs)
{
}

HRESULT CUI_CharInterface::Ready_GameObject(void* pArg)
{
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fX = 850;
	m_fY = 325;
	m_fSizeX = 200.f;
	m_fSizeY = 250.f;

	return S_OK;
}

_int CUI_CharInterface::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
{
	return _int();
}

_int CUI_CharInterface::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CUI_CharInterface::Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture)
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

	ComPtr<ID3D12DescriptorHeap>	pTextureDesc = pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->Get_RTT(0)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc.Get(), TEXTURE_REGISTER::t0);
	//CDevice::GetInstance()->SetTextureToShader(texture->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	buffer->Render_VIBuffer();

	Safe_Release(pManagement);
}
