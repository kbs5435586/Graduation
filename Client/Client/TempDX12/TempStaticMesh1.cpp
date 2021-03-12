#include "framework.h"
#include "TempStaticMesh1.h"
#include "Management.h"
#include "Hierachy_Loader.h"
CTempStaticMesh1::CTempStaticMesh1(ID3D12Device* pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CTempStaticMesh1::CTempStaticMesh1(const CTempStaticMesh1& rhs)
	: CGameObject(rhs)
{

}

HRESULT CTempStaticMesh1::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTempStaticMesh1::Ready_GameObject(void* pArg)
{
	m_eRootType = ROOT_TYPE_TEXTURE;
	m_iPassSize = CalcConstantBufferByteSize(sizeof(MAINPASS_LIGHT));


	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateConstantBuffer()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Compile_Shader()))
		return E_FAIL;
	if (FAILED(CreatePipeLine(m_pShaderCom)))
		return E_FAIL;

	if (m_pTextureCom != nullptr)
	{
		if (FAILED(m_pTextureCom->Create_ShaderResourceView(0)))
			return E_FAIL;
	}
	//if (m_pTextureCom1 != nullptr)
	//{
	//	if (FAILED(m_pTextureCom1->Create_ShaderResourceView(1)))
	//		return E_FAIL;
	//}


	_vec3 vPos = _vec3(30.f, 0.f, 30.f);
	m_pTransformCom->SetUp_Speed(30.f, XMConvertToRadians(30.f));
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);


	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);


	return S_OK;
}

_int CTempStaticMesh1::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Rotation_Y(fTimeDelta);

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY);

	Safe_Release(pManagement);
	return _int();
}

_int CTempStaticMesh1::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;


	return _int();
}

void CTempStaticMesh1::Render_GameObject()
{
	CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(m_eRootType));
	if (nullptr != m_pMeshCom && nullptr != m_pConstBuffer)
	{
		MAINPASS_LIGHT tPass;
		if (FAILED(Ready_Light(tPass)))
			return;
		m_pTextureCom->SetUp_OnShader(CDevice::GetInstance()->GetCommandList());
		CDevice::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, m_pConstBuffer->GetGPUVirtualAddress());
		m_pMeshCom->Render_Mesh(m_pPipeLine, m_pShaderCom, m_pTransformCom->Get_Matrix(), tPass, m_iPassSize, m_pData, m_eRootType);
	}
}

HRESULT CTempStaticMesh1::CreatePipeLine(CShader* pShader)
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

D3D12_RASTERIZER_DESC CTempStaticMesh1::CreateRaterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
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

D3D12_DEPTH_STENCIL_DESC CTempStaticMesh1::CreateDepthStencilState()
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

D3D12_BLEND_DESC CTempStaticMesh1::CreateBlendState()
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

D3D12_INPUT_LAYOUT_DESC CTempStaticMesh1::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	//pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

HRESULT CTempStaticMesh1::Ready_Light(MAINPASS_LIGHT& tInfo)
{
	CLight_Manager* pLight_Manager = CLight_Manager::GetInstance();
	if (nullptr == pLight_Manager)
		return E_FAIL;
	pLight_Manager->AddRef();

	LIGHT tLight = pLight_Manager->GetLight(L"Light_Default");

	tInfo.vLightDirection = tLight.vDirection;
	tInfo.vLightDiffuse = tLight.vDiffuse;
	tInfo.vLightSpecular = tLight.vSpecular;
	tInfo.vLightAmbient = tLight.vAmbient;
	tInfo.fPower = 30.f;

	tInfo.vMaterialDiffuse = m_pMeshCom->GetLoader()->GetRenderInfo()[0]->vecMtrlInfo[0].vMtrlDiff;
	tInfo.vMaterialSpecular = m_pMeshCom->GetLoader()->GetRenderInfo()[0]->vecMtrlInfo[0].vMtrlSpec;
	tInfo.vMaterialAmbient = m_pMeshCom->GetLoader()->GetRenderInfo()[0]->vecMtrlInfo[0].vMtrlAmb;

	tInfo.vCameraPos = (_vec4)CDevice::GetInstance()->GetViewMatrix().m[3];

	Safe_Release(pLight_Manager);
	return S_OK;
}

HRESULT CTempStaticMesh1::CreateConstantBuffer()
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

CTempStaticMesh1* CTempStaticMesh1::Create(ID3D12Device* pGraphic_Device)
{
	CTempStaticMesh1* pInstance = new CTempStaticMesh1(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTempMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTempStaticMesh1::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	CTempStaticMesh1* pInstance = new CTempStaticMesh1(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CTempMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}

	m_iLayerIdx = iIdx;

	return pInstance;
}

void CTempStaticMesh1::Free()
{
	if (m_pConstBuffer)
		m_pConstBuffer->Release();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	CGameObject::Free();
}

HRESULT CTempStaticMesh1::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Hatching");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pMeshCom = (CStatic_Mesh*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp1");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Texture_Hatching123");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}
