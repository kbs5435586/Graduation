#include "framework.h"
#include "Shader.h"

CShader::CShader(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pFilePath(rhs.m_pFilePath)
	, m_pPixelShader(rhs.m_pPixelShader)
	, m_pVertexShader(rhs.m_pVertexShader)
	, m_pVSEntryPoint(rhs.m_pVSEntryPoint)
	, m_pPSEntryPoint(rhs.m_pPSEntryPoint)
{

}

HRESULT CShader::Ready_Shader(const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag)
{
	m_pFilePath = pFilepath;
	m_pVSEntryPoint = VSEntry;
	m_pPSEntryPoint = PSEntry;
	m_iCompileFlags = iFlag;
	return S_OK;
}

HRESULT CShader::Compile_Shader()
{
	ID3DBlob* compilationMsgs = nullptr;
	if (FAILED(D3DCompileFromFile(m_pFilePath, nullptr, nullptr, m_pVSEntryPoint, "vs_5_1", m_iCompileFlags, 0, &m_pVertexShader, &compilationMsgs)))
	{
		char* pTemp = ((char*)compilationMsgs->GetBufferPointer());
		return E_FAIL;
	}


	if (FAILED(D3DCompileFromFile(m_pFilePath, nullptr, nullptr, m_pPSEntryPoint, "ps_5_1", m_iCompileFlags, 0, &m_pPixelShader, &compilationMsgs)))
	{
		char* pTemp = ((char*)compilationMsgs->GetBufferPointer());
		return E_FAIL;
	}	
	return S_OK;
}

HRESULT CShader::SetUp_OnShader(ID3D12PipelineState* pPipeline, ID3D12Resource* pConstantBuffer, _matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& output, ROOT_TYPE eType)
{
	CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(eType));
	CDevice::GetInstance()->GetCommandList()->SetPipelineState(pPipeline);

	
	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&matWorld));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	output.matWorld = xmMatWorld;
	output.matView = xmMatView;
	output.matProj = xmMatProj;

	return S_OK;
}

CShader* CShader::Create(ID3D12Device* pGraphic_Device, const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag)
{
	CShader* pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Shader(pFilepath, VSEntry, PSEntry, iFlag)))
	{
		MessageBox(0, L"CShader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone_Component(void* pArg)
{
	return new CShader(*this);
}

void CShader::Free()
{
	CComponent::Free();
}
