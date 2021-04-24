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
	, m_tPipeline_CS(rhs.m_tPipeline_CS)
	, m_pPilelineState_CS(rhs.m_pPilelineState_CS)
{
}

HRESULT CShader::Ready_Shader(const _tchar* pFilePath, const char* VSEntry,
	const char* PSEntry, const char* GSEntry, const char* DSEntry, const char* HSEntry)
{
	char* pErr = nullptr;
	ZeroMemory(&m_tPipeline, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	if (VSEntry)
	{
		if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, VSEntry, "vs_5_1", 0, 0, &m_pVSBlob, &m_pErrBlob)))
		{
			pErr = (char*)m_pErrBlob->GetBufferPointer();
			MessageBoxA(nullptr, pErr, "VS_Shader Create Failed !!!", MB_OK);
			return E_FAIL;
		}

		m_tPipeline.VS = { m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize() };
	}

	if (PSEntry)
	{
		if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, PSEntry, "ps_5_1", 0, 0, &m_pPSBlob, &m_pErrBlob)))
		{
			pErr = (char*)m_pErrBlob->GetBufferPointer();
			MessageBoxA(nullptr, pErr, "PS_Shader Create Failed !!!", MB_OK);
			return E_FAIL;
		}
		m_tPipeline.PS = { m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize() };
	}
	if (GSEntry)
	{
		if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, GSEntry, "gs_5_1", 0, 0, &m_pGSBlob, &m_pErrBlob)))
		{
			pErr = (char*)m_pErrBlob->GetBufferPointer();
			MessageBoxA(nullptr, pErr, "GS_Shader Create Failed !!!", MB_OK);
			return E_FAIL;
		}
		m_tPipeline.GS = { m_pGSBlob->GetBufferPointer(), m_pGSBlob->GetBufferSize() };
	}

	return S_OK;
}

HRESULT CShader::Create_Shader(vector< D3D12_INPUT_ELEMENT_DESC> vecDesc, RS_TYPE eType, DEPTH_STENCIL_TYPE eDepthType, SHADER_TYPE eShaderType, BLEND_TYPE eBlendType, D3D_PRIMITIVE_TOPOLOGY eTopology)
{
	m_tPipeline.InputLayout = { vecDesc.data(), (_uint)vecDesc.size() };
	m_tPipeline.pRootSignature = CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get();

	m_tPipeline.RasterizerState = g_arrRSDesc[(UINT)eType];
	m_tPipeline.BlendState = g_arrBlendDesc[(UINT)eBlendType];

	m_tPipeline.DepthStencilState = g_arrDepthStencilDesc[(UINT)eDepthType];





	m_tPipeline.SampleMask = UINT_MAX;
	m_tPipeline.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_tPipeline.SampleDesc.Count = 1;


	switch (eShaderType)
	{
	case SHADER_TYPE::SHADER_FORWARD:
		m_tPipeline.NumRenderTargets = 1;
		m_tPipeline.RTVFormats[0] = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());
		break;
	case SHADER_TYPE::SHADER_DEFFERED:
		m_tPipeline.NumRenderTargets = 5;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_tPipeline.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_tPipeline.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_tPipeline.RTVFormats[4] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		break;
	case SHADER_TYPE::SHADER_LIGHT:
		m_tPipeline.NumRenderTargets = 2;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::SHADER_COMPUTE:
		m_tPipeline.NumRenderTargets = 0;
		break;
	case SHADER_TYPE::SHADER_PARTICLE:
		m_tPipeline.NumRenderTargets = 1;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	switch (eTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		break;
	default:
		assert(nullptr);
	}

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&m_tPipeline, IID_PPV_ARGS(&m_pPipeLineState))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShader::Ready_Shader(const _tchar* pFilePath, const char* CSEntry)
{
	char* pErr = nullptr;
	ZeroMemory(&m_tPipeline_CS, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));

	if (CSEntry)
	{
		if (FAILED(D3DCompileFromFile(pFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, CSEntry, "cs_5_1", 0, 0, &m_pCSBlob, &m_pErrBlob)))
		{
			pErr = (char*)m_pErrBlob->GetBufferPointer();
			MessageBoxA(nullptr, pErr, "CS_Shader Create Failed !!!", MB_OK);
			return E_FAIL;
		}
		m_tPipeline_CS.pRootSignature = CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::COMPUTE).Get();
		m_tPipeline_CS.CS = { m_pCSBlob->GetBufferPointer(), m_pCSBlob->GetBufferSize() };

		CDevice::GetInstance()->GetDevice()->CreateComputePipelineState(&m_tPipeline_CS, IID_PPV_ARGS(&m_pPilelineState_CS));
	}

	return S_OK;
}

HRESULT CShader::SetUp_OnShader(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& output)
{
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pPipeLineState.Get());
	_matrix	matTemp = matView;
	matTemp = Matrix_::Inverse(matTemp);
	_matrix matRev = _matrix();

	output.matWorld = matWorld;
	output.matView = matView;
	output.matProj = matProj;
	output.matWV = matWorld * matView;
	output.matWVP = output.matWV * matProj;
	output.vCamPos = (_vec4)&matTemp.m[3][0];
	output.matRev = matRev;

	output.matViewInv = Matrix_::Inverse(matView);
	output.matProjInv = Matrix_::Inverse(matProj);
	return S_OK;
}

HRESULT CShader::SetUp_OnShader_FbxMesh(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& tPass)
{
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pPipeLineState.Get());

	_matrix	matTemp = matView;
	matTemp = Matrix_::Inverse(matTemp);

	tPass.matWorld = matWorld;
	tPass.matView = matView;
	tPass.matProj = matProj;
	tPass.matWV = matWorld * matView;
	tPass.matWVP = tPass.matWV * matProj;
	tPass.vCamPos = (_vec4)&matTemp.m[3][0];
	return S_OK;
}

void CShader::UpdateData_CS()
{
	CDevice::GetInstance()->GetCsCmdLst()->SetPipelineState(m_pPilelineState_CS.Get());
}

CShader* CShader::Create(const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, const char* GSEntry, const char* DSEntry, const char* HSEntry)
{
	CShader* pInstance = new CShader();

	if (FAILED(pInstance->Ready_Shader(pFilepath, VSEntry, PSEntry, GSEntry, DSEntry, HSEntry)))
	{
		MessageBox(0, L"CShader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CShader* CShader::Create(const _tchar* pFilePath, const char* CSEntry)
{
	CShader* pInstance = new CShader();

	if (FAILED(pInstance->Ready_Shader(pFilePath, CSEntry)))
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
