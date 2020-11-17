#include "framework.h"
#include "TempMesh.h"
#include "Management.h"

CTempMesh::CTempMesh(ID3D12Device* pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTempMesh::CTempMesh(const CTempMesh& rhs)
	: CGameObject(rhs)
{

}

HRESULT CTempMesh::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTempMesh::Ready_GameObject(void* pArg)
{
	m_eRootType = ROOT_TYPE_COLOR;
	m_iPassSize = CalcConstantBufferByteSize(sizeof(MAINPASS));

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateConstantBuffer()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Compile_Shader()))
		return E_FAIL;
	if (FAILED(CreatePipeLine(m_pShaderCom)))
		return E_FAIL;

	_vec3 vPos = _vec3(-0.f, 0.f, 0.f);
	m_pTransformCom->SetUp_Speed(30.f, XMConvertToRadians(30.f));
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);

	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	//m_pTransformCom->Scaling(0.01f, 0.01f, 0.01f);
	if (nullptr != m_pMeshCom)
		m_pMeshCom->SetAnimationStack(0);
	return S_OK;
}

_int CTempMesh::Update_GameObject(const _float& fTimeDelta)
{
	if (nullptr != m_pMeshCom)
	{
		m_pMeshCom->Animate(fTimeDelta);
	}

	return _int();
}

_int CTempMesh::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;


	return _int();
}

void CTempMesh::Render_GameObject()
{

	if (nullptr != m_pMeshCom && nullptr != m_pConstBuffer)
	{
		CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(m_eRootType));
		CDevice::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0,m_pConstBuffer->GetGPUVirtualAddress());
		m_pMeshCom->Render_Mesh(m_pPipeLine, m_pShaderCom, m_pTransformCom->Get_Matrix(), m_iPassSize, m_pData, m_eRootType);
	}

}

HRESULT CTempMesh::CreatePipeLine(CShader* pShader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.pRootSignature = CDevice::GetInstance()->GetRootSignature(m_eRootType);
	d3dPipelineStateDesc.VS.pShaderBytecode = pShader->GetVertexShader()->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pShader->GetVertexShader()->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pShader->GetPixelShader()->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pShader->GetPixelShader()->GetBufferSize();
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	d3dPipelineStateDesc.RasterizerState = CreateRaterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();

	d3dPipelineStateDesc.SampleMask = 0xffffffff;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.NumRenderTargets = 1;

	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pPipeLine)))
		return E_FAIL;
	m_pPipeLine->SetName(L"PipeLine");

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
	return S_OK;
}

D3D12_RASTERIZER_DESC CTempMesh::CreateRaterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CTempMesh::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CTempMesh::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CTempMesh::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

HRESULT CTempMesh::CreateConstantBuffer()
{
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iPassSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&tHeap_Pro_Upload,
		D3D12_HEAP_FLAG_NONE,
		&tResource_Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pConstBuffer))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pConstBuffer->Map(0, nullptr, &m_pData)))
		return E_FAIL;

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferAddress = m_pConstBuffer->GetGPUVirtualAddress();

	int Idx = 0;
	ConstantBufferAddress += (Idx * m_iPassSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = ConstantBufferAddress;
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(MAINPASS));

	CDevice::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, CDevice::GetInstance()->GetConstantBufferDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

CTempMesh* CTempMesh::Create(ID3D12Device* pGraphic_Device)
{
	CTempMesh* pInstance = new CTempMesh(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTempMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTempMesh::Clone_GameObject(void* pArg)
{
	CTempMesh* pInstance = new CTempMesh(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CTempMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTempMesh::Free()
{
	if (m_pConstBuffer)
		m_pConstBuffer->Release();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	CGameObject::Free();
}

HRESULT CTempMesh::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Mesh");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pMeshCom = (CDynamic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
