#include "stdafx.h"
#include "MyFont.h"
#include "Transform.h"
CMyFont::CMyFont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CMyFont::CMyFont(const CMyFont & rhs)
	: CComponent(rhs)
	, m_pSprite(rhs.m_pSprite)
	, m_pFont(rhs.m_pFont)
{
	m_pFont->AddRef();
	m_pSprite->AddRef();
}

HRESULT CMyFont::Ready_Font()
{
	if (FAILED(D3DXCreateFont(m_pGraphic_Device, 20, 20, FW_BOLD, 0,FALSE, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE, TEXT("±Ã¼­"),
		&m_pFont)))
		return E_FAIL;
	if (FAILED(D3DXCreateSprite(m_pGraphic_Device, &m_pSprite)))
		return E_FAIL;

	m_pFont->AddRef();
	m_pSprite->AddRef();

	return S_OK;
}

HRESULT CMyFont::SetUp_3DFont(const char * pStr, CTransform * pTransform, int r, int g, int b)
{
	if (m_pSprite == nullptr)
		D3DXCreateSprite(m_pGraphic_Device, &m_pSprite);

	_matrix matOut;
	D3DXMatrixIdentity(&matOut);
	_vec3 vPos, temp;
	vPos = *pTransform->Get_StateInfo(STATE_POSITION);

	temp = GetScreenPos(vPos);

	D3DXMatrixTranslation(&matOut, temp.x, temp.y, temp.z);

	m_pSprite->SetTransform(&matOut);
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	rt = { 0,0,0,0 };

	if (FAILED(m_pFont->DrawTextA(m_pSprite, pStr, -1, &rt, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b))))
		return E_FAIL;

	m_pSprite->End();
	return S_OK;
}

CMyFont * CMyFont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMyFont* pInstance = new CMyFont(pGraphic_Device);
	if (FAILED(pInstance->Ready_Font()))
		return nullptr;
	return pInstance;
}

CComponent * CMyFont::Clone_Component(void* pArg)
{
	return new CMyFont(*this);
}

void CMyFont::Free()
{
	Safe_Release(m_pSprite);
	Safe_Release(m_pFont);

	CComponent::Free();
}

D3DXVECTOR3 CMyFont::GetScreenPos(_vec3& vPos)
{
	D3DXMATRIX matView, matProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMATRIX mat = matView * matProj;

	D3DXVECTOR3 point;

	D3DXVec3TransformCoord(&point, &vPos, &mat);


	point.x = (point.x + 1) * g_iBackCX / 2;
	point.y = (-point.y + 1) * g_iBackCY / 2;

	return point;
}
