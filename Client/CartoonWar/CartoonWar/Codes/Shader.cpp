#include "framework.h"
#include "Shader.h"

CShader::CShader()
{
}

CShader::CShader(const CShader& rhs)
	: m_pVSBlob(rhs.m_pVSBlob)
	, m_pPSBlob(rhs.m_pPSBlob)
	, m_pHSBlob(rhs.m_pHSBlob)
	, m_pDSBlob(rhs.m_pDSBlob)
	, m_pCSBlob(rhs.m_pCSBlob)
	, m_pGSBlob(rhs.m_pGSBlob)
	, m_tPipeline(rhs.m_tPipeline)
{
}

HRESULT CShader::Ready_Shader(const _tchar* pFilePath, const char* VSEntry, 
	const char* PSEntry, const char* HSEntry, const char* DSEntry, const char* GSEntry)
{
	char* pErr = nullptr;

	if (nullptr == VSEntry || nullptr == PSEntry)
		return E_FAIL;

	if (FAILED(D3DCompileFromFile(pFilePath, nullptr, nullptr
		, VSEntry, "vs_5_1", 0, 0, &m_pVSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "VS_Shader Create Failed !!!", MB_OK);
		return E_FAIL;
	}
	

	if (FAILED(D3DCompileFromFile(pFilePath, nullptr, nullptr
		, PSEntry, "ps_5_1", 0, 0, &m_pPSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "PS_Shader Create Failed !!!", MB_OK);
		return E_FAIL;
	}


	m_tPipeline.VS = { m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize() };
	m_tPipeline.PS = { m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize() };
	
	return S_OK;
}

HRESULT CShader::Create_Shader(vector< D3D12_INPUT_ELEMENT_DESC> vecDesc, RS_TYPE eType)
{
	m_tPipeline.InputLayout = { vecDesc.data(), (_uint)vecDesc.size() };
	m_tPipeline.pRootSignature = CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get();

	m_tPipeline.RasterizerState = g_arrRSDesc[(UINT)eType];
	m_tPipeline.BlendState = g_arrBlendDesc[(UINT)BLEND_TYPE::DEFAULT];
	m_tPipeline.DepthStencilState.DepthEnable = FALSE;
	m_tPipeline.DepthStencilState.StencilEnable = FALSE;
	m_tPipeline.SampleMask = UINT_MAX;
	m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_tPipeline.NumRenderTargets = 1;
	m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_tPipeline.SampleDesc.Count = 1;

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&m_tPipeline,
		IID_PPV_ARGS(&m_pPipeLineState))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShader::SetUp_OnShader(ID3D12Resource* pConstantBuffer, _matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& output)
{
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pPipeLineState.Get());


	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&matWorld));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	output.matWorld = xmMatWorld;
	output.matView = xmMatView;
	output.matProj = xmMatProj;

	return S_OK;
}

CShader* CShader::Create(const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, const char* HSEntry, const char* DSEntry, const char* GSEntry)
{
	CShader* pInstance = new CShader();

	if (FAILED(pInstance->Ready_Shader(pFilepath, VSEntry, PSEntry)))
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
