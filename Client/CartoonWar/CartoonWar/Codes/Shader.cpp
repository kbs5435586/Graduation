#include "framework.h"
#include "Shader.h"

CShader::CShader()
	: CComponent()
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pVSBlob(rhs.m_pVSBlob)
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

	if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, VSEntry, "vs_5_1", 0, 0, &m_pVSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "VS_Shader Create Failed !!!", MB_OK);
		return E_FAIL;
	}
	

	if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, PSEntry, "ps_5_1", 0, 0, &m_pPSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "PS_Shader Create Failed !!!", MB_OK);
		return E_FAIL;
	}

	ZeroMemory(&m_tPipeline, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_tPipeline.VS = { m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize() };
	m_tPipeline.PS = { m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize() };
	
	return S_OK;
}

HRESULT CShader::Create_Shader(vector< D3D12_INPUT_ELEMENT_DESC> vecDesc, RS_TYPE eType, SHADER_TYPE eShaderType)
{
	m_tPipeline.InputLayout = { vecDesc.data(), (_uint)vecDesc.size() };
	m_tPipeline.pRootSignature = CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get();

	m_tPipeline.RasterizerState = g_arrRSDesc[(UINT)eType];
	m_tPipeline.BlendState = g_arrBlendDesc[(UINT)BLEND_TYPE::DEFAULT];

	m_tPipeline.DepthStencilState.DepthEnable = TRUE;
	m_tPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_tPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	m_tPipeline.DepthStencilState.StencilEnable = FALSE;
	m_tPipeline.DepthStencilState.StencilReadMask = 0x00;
	m_tPipeline.DepthStencilState.StencilWriteMask = 0x00;

	m_tPipeline.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	m_tPipeline.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_tPipeline.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;


	m_tPipeline.SampleMask = UINT_MAX;
	m_tPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	m_tPipeline.SampleDesc.Count = 1;

	switch (eShaderType)
	{
	case SHADER_TYPE::SHADER_FORWARD:
		m_tPipeline.NumRenderTargets = 1;
		m_tPipeline.RTVFormats[0] = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());
		break;
	case SHADER_TYPE::SHADER_DEFFERD:
		m_tPipeline.NumRenderTargets = 3;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_tPipeline.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case SHADER_TYPE::SHADER_LIGHT:
		m_tPipeline.NumRenderTargets = 2;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}


	m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&m_tPipeline, IID_PPV_ARGS(&m_pPipeLineState))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShader::SetUp_OnShader(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& output)
{
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pPipeLineState.Get());

	output.matWorld		= matWorld;
	output.matView		= matView;
	output.matProj		= matProj;
	output.matWV		= matWorld * matView;
	output.matWVP		= output.matWV * matProj;

	return S_OK;
}

HRESULT CShader::SetUp_OnShader_FbxMesh(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& tPass)
{
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pPipeLineState.Get());

	XMMATRIX	xmMatWorld	= XMLoadFloat4x4(&matWorld);
	XMMATRIX	xmMatView	= XMLoadFloat4x4(&matView);
	XMMATRIX	xmMatProj	= XMLoadFloat4x4(&matProj);

	tPass.matWorld	= matWorld;
	tPass.matView	= matView;
	tPass.matProj	= matProj;
	tPass.matWV		= matWorld * matView;
	tPass.matWVP	= tPass.matWV * matProj;

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
