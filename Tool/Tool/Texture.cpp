#include "stdafx.h"
#include "Texture.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_vecTexture(rhs.m_vecTexture)
{
	for (auto& pTexture : m_vecTexture)
		pTexture->AddRef();
	m_pComponent_Tag = rhs.m_pComponent_Tag;
}

HRESULT CTexture::Ready_Texture(TEXTURE_TYPE eType, const _tchar * pFilePath, const _uint & iNumTexture)
{
	m_vecTexture.reserve(iNumTexture);

	for (size_t i = 0; i < iNumTexture; i++)
	{
		LPDIRECT3DTEXTURE9		pTexture = nullptr;
		_tchar					szFilePath[MAX_PATH] = L"";

		wsprintf(szFilePath, pFilePath, i);

		if (TEXTURE_TYPE_GENERAL == eType)
		{
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFilePath, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;
		}
		else if (TEXTURE_TYPE_CUBE == eType)
		{
			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szFilePath, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;
		}

		m_vecTexture.push_back(pTexture);
	}
	return S_OK;
}

HRESULT CTexture::SetUp_OnGraphicDev(const _uint & iIndex)
{
	if (m_vecTexture.size() <= iIndex)
		return E_FAIL;
	if (FAILED(m_pGraphic_Device->SetTexture(0, m_vecTexture[iIndex])))
		return E_FAIL;
	return S_OK;
}

HRESULT CTexture::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName, const _uint & iIdx)
{
	if (m_vecTexture.size() <= iIdx)
		return E_FAIL;
	if (FAILED(pEffect->SetTexture(pConstantName, m_vecTexture[iIdx])))
		return E_FAIL;

	return S_OK;
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURE_TYPE eType, const _tchar * pFilePath, const _uint & iNumTexture)
{
	CTexture*	pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Ready_Texture(eType, pFilePath, iNumTexture)))
	{
		MessageBox(0, L"CTexture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone_Component(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pComponent_Tag = (_tchar*)pArg;
	}

	return new CTexture(*this);
}

void CTexture::Free()
{
	for (auto& pTexture : m_vecTexture)
		Safe_Release(pTexture);

	m_vecTexture.clear();

	CComponent::Free();
}
