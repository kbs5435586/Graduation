#include "stdafx.h"
#include "Shader.h"


CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pBuffer(rhs.m_pBuffer)
{
	if (nullptr != m_pEffect)
		m_pEffect->AddRef();

	if (nullptr != m_pBuffer)
		m_pBuffer->AddRef();
}

HRESULT CShader::Ready_Shader(const _tchar * pFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, pFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, &m_pBuffer)))
		return E_FAIL;
	return S_OK;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
{
	CShader*	pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Shader(pFilePath)))
	{
		MessageBox(0, L"CShader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone_Component(void* pArg)
{
	return new CShader(*this);
}

void CShader::Free()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pEffect);

	CComponent::Free();
}
