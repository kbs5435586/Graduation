#include "framework.h"
#include "Device.h"
#include "Renderer.h"
_IMPLEMENT_SINGLETON(CDevice);
CDevice::CDevice()
{
}

HRESULT CDevice::Initialize()
{
	for (int i = 0; i < m_iSwapchainBuffer; ++i)
	{
		m_iFenceValues[i] = 0;
		m_pRenderTargetBuffers[i] = nullptr;
	}

	return S_OK;
}

HRESULT CDevice::OnCreate()
{
	if (FAILED(CreateDevice()))
		return E_FAIL;
	if (FAILED(CreateCommandQueueAndList()))
		return E_FAIL;
	if (FAILED(CreateSwapChain()))
		return E_FAIL;
	if (FAILED(CreateDescriptorHeap()))
		return E_FAIL;
	if (FAILED(CreateRenderTargerView()))
		return E_FAIL;
	if (FAILED(CreateDepthStencilView()))
		return E_FAIL;
	if (FAILED(CreateRootSignature(ROOT_TYPE_COLOR)))
		return E_FAIL;
	if (FAILED(CreateRootSignature(ROOT_TYPE_TEXTURE)))
		return E_FAIL;
	if (FAILED(CreateRootSignature(ROOT_TYPE_MULTI_TEXTURE)))
		return E_FAIL;
	/*if (FAILED(CreateRootSignature(1)))
		return E_FAIL;*/
	m_pDevice->SetName(L"Graphic_Device");
	for (int i = 0; i < m_iSwapchainBuffer; ++i)
		m_pRenderTargetBuffers[i]->SetName(L"Render Target Buffer");
	m_pDepthStencilBuffer->SetName(L"Depth Stencil Buffer");
	m_pRtvDescriptorHeap->SetName(L"Render Target View Descriptor Heap");
	m_pDsvDescriptorHeap->SetName(L"Depth Stencil View Descriptor Heap");
	m_pCbvDescriptorHeap->SetName(L"Constant Buffer View Descriptor Heap");
	m_pCommandAllocator->SetName(L"Command Allocator");
	m_pCommandQueue->SetName(L"CommandQueue");
	m_pCommandList->SetName(L"CommandList");

	for (auto& iter : m_vecRootSignature)
		iter->SetName(L"RootSignature");

	return S_OK;
}

HRESULT CDevice::OnDestroy()
{
	WaitForGpuComplete();
	CloseHandle(m_hFenceEvent);
	if (m_pDepthStencilBuffer)
		m_pDepthStencilBuffer->Release();
	if (m_pDsvDescriptorHeap)
		m_pDsvDescriptorHeap->Release();
	for (int i = 0; i < m_iSwapchainBuffer; ++i)
	{
		if (m_pRenderTargetBuffers[i])
			m_pRenderTargetBuffers[i]->Release();
	}

	if (m_pRtvDescriptorHeap)
		m_pRtvDescriptorHeap->Release();
	if (m_pCbvDescriptorHeap)
		m_pCbvDescriptorHeap->Release();

	for (auto& iter : m_vecRootSignature)
		Safe_Release(iter);

	if (m_pCommandAllocator)
		m_pCommandAllocator->Release();
	if (m_pCommandQueue)
		m_pCommandQueue->Release();
	if (m_pCommandList)
		m_pCommandList->Release();

	if (m_pFence)
		m_pFence->Release();
	m_pSwapChain->SetFullscreenState(FALSE, nullptr);
	if (m_pSwapChain)
		m_pSwapChain->Release();
	if (m_pDevice)
		m_pDevice->Release();
	if (m_pFactory)
		m_pFactory->Release();
#if defined(_DEBUG)
	if (m_pDebugController)
		m_pDebugController->Release();
#endif
	return S_OK;
}

HRESULT CDevice::CreateDevice()
{

#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&m_pDebugController);
	m_pDebugController->EnableDebugLayer();
#endif
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_pFactory);
	IDXGIAdapter1* pAdapter = nullptr;
	for (_uint i = 0; DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, &pAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc;
		pAdapter->GetDesc1(&AdapterDesc);
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pDevice)))
			break;
	}
	if (!pAdapter)
	{
		m_pFactory->EnumWarpAdapter(__uuidof(IDXGIFactory4), (void**)&m_pFactory);
		D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pDevice);
	}
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MsaaQualityLevels;
	MsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MsaaQualityLevels.SampleCount = 4;
	MsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MsaaQualityLevels.NumQualityLevels = 0;
	m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_iMsaaQualityLv = MsaaQualityLevels.NumQualityLevels;
	m_bMsaaEnalbe = (m_iMsaaQualityLv > 1) ? true : false;

	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pFence);

	for (int i = 0; i < m_iSwapchainBuffer; i++)
		m_iFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(WINCX);
	m_ViewPort.Height = static_cast<float>(WINCY);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ScissorRect = { 0, 0, WINCX, WINCY };
	if (pAdapter)
		pAdapter->Release();

	return S_OK;
}

HRESULT CDevice::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	ZeroMemory(&CommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(m_pDevice->CreateCommandQueue(&CommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pCommandQueue)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pCommandAllocator)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, __uuidof(ID3D12CommandList), (void**)&m_pCommandList)))
		return E_FAIL;

	Close();

	return S_OK;
}

HRESULT CDevice::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = m_iSwapchainBuffer;
	SwapChainDesc.BufferDesc.Width = (WINCX);
	SwapChainDesc.BufferDesc.Height = (WINCY);
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 120;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.OutputWindow = g_hWnd;
	SwapChainDesc.SampleDesc.Count = (m_bMsaaEnalbe) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (m_bMsaaEnalbe) ? (m_iMsaaQualityLv - 1) : 0;
	SwapChainDesc.Windowed = TRUE;

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	//전체화면 모드에서 바탕화면의 해상도를 바꾸지 않고 후면버퍼의 크기를 바탕화면 크기로 변경한다.
	SwapChainDesc.Flags = 0;
#else
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	if (FAILED(m_pFactory->CreateSwapChain(m_pCommandQueue, &SwapChainDesc, (IDXGISwapChain**)&m_pSwapChain)))
		return E_FAIL;
	return S_OK;
}

HRESULT CDevice::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescHeap;
	ZeroMemory(&DescHeap, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	//RenderTargetView의 Heap영역 생성
	DescHeap.NumDescriptors = m_iSwapchainBuffer;
	DescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescHeap.NodeMask = 0;
	if (FAILED(m_pDevice->CreateDescriptorHeap(&DescHeap, __uuidof(ID3D12DescriptorHeap), (void**)&m_pRtvDescriptorHeap)))
		return E_FAIL;
	m_iRtvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//DepthStencilView의 Heap영역 생성
	DescHeap.NumDescriptors = 1;
	DescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(m_pDevice->CreateDescriptorHeap(&DescHeap, __uuidof(ID3D12DescriptorHeap), (void**)&m_pDsvDescriptorHeap)))
		return E_FAIL;
	m_iDsvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//Create Constant 
	DescHeap.NumDescriptors = 1;
	DescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescHeap.NodeMask = 0;
	if (FAILED(m_pDevice->CreateDescriptorHeap(&DescHeap, IID_PPV_ARGS(&m_pCbvDescriptorHeap))))
		return E_FAIL;
	m_iCbvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Create Shader Resource Heap

	return S_OK;
}

HRESULT CDevice::CreateRenderTargerView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (_uint i = 0; i < m_iSwapchainBuffer; ++i)
	{
		if (FAILED(m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_pRenderTargetBuffers[i])))
			return E_FAIL;
		m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffers[i], nullptr, RtvHandle);
		RtvHandle.ptr += m_iRtvDescriptorIncrementSize;
	}
	return S_OK;
}

HRESULT CDevice::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = WINCX;
	ResourceDesc.Height = WINCX;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.SampleDesc.Count = (m_bMsaaEnalbe) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (m_bMsaaEnalbe) ? (m_iMsaaQualityLv - 1) : 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;
	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	if (FAILED(m_pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pDepthStencilBuffer)))
		return E_FAIL;

	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, DsvCPUDescriptorHandle);
	return S_OK;
}

array<const CD3DX12_STATIC_SAMPLER_DESC, 6> CDevice::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

void CDevice::WaitForGpuComplete()
{
	_uint nFenceValue = ++m_iFenceValues[m_iSwapChainBufferIdx];
	if (FAILED(m_pCommandQueue->Signal(m_pFence, nFenceValue)))
		return;

	if (m_pFence->GetCompletedValue() < nFenceValue)
	{
		if (FAILED(m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent)))
			return;
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CDevice::MoveToNextFrame()
{
	m_iSwapChainBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
	_uint nFenceValue = ++m_iFenceValues[m_iSwapChainBufferIdx];
	if (FAILED(m_pCommandQueue->Signal(m_pFence, nFenceValue)))
		return;
	if (m_pFence->GetCompletedValue() < nFenceValue)
	{
		if (FAILED(m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent)))
			return;
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

HRESULT CDevice::CreateRootSignature(ROOT_TYPE eType)
{
	ID3D12RootSignature* pRootSignature;

	if (eType == ROOT_TYPE_COLOR)
	{
		D3D12_ROOT_PARAMETER shaderParams[1] = { };
		D3D12_ROOT_SIGNATURE_DESC signature = { };

		shaderParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		shaderParams[0].Constants.ShaderRegister = 0;  // register(b0)



		signature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		signature.NumParameters = _countof(shaderParams);
		signature.pParameters = shaderParams;
		signature.NumStaticSamplers = 0;

		ID3DBlob* pRootSignatureCode = nullptr;
		ID3DBlob* pError = nullptr;
		if (FAILED(D3D12SerializeRootSignature(&signature, D3D_ROOT_SIGNATURE_VERSION_1, &pRootSignatureCode, &pError)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateRootSignature(0, pRootSignatureCode->GetBufferPointer(), pRootSignatureCode->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))))
			return E_FAIL;

		m_vecRootSignature.push_back(pRootSignature);
	}
	else if (eType == ROOT_TYPE_TEXTURE)
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[2] = {};

		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);

		auto staticSamplers = GetStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ID3DBlob* pRootSignatureCode = nullptr;
		ID3DBlob* pError = nullptr;
		if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pRootSignatureCode, &pError)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateRootSignature(0, pRootSignatureCode->GetBufferPointer(), pRootSignatureCode->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))))
			return E_FAIL;
		m_vecRootSignature.push_back(pRootSignature);
	}
	else if (eType == ROOT_TYPE_MULTI_TEXTURE)
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0);


		CD3DX12_ROOT_PARAMETER slotRootParameter[2] = {};
		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);

		auto staticSamplers = GetStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ID3DBlob* pRootSignatureCode = nullptr;
		ID3DBlob* pError = nullptr;
		if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pRootSignatureCode, &pError)))
		{
			char* pTemp = ((char*)pError->GetBufferPointer());
			return E_FAIL;
		}


		if (FAILED(m_pDevice->CreateRootSignature(0, pRootSignatureCode->GetBufferPointer(), pRootSignatureCode->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))))
			return E_FAIL;
		m_vecRootSignature.push_back(pRootSignature);
	}


	return S_OK;
}


void CDevice::Open()
{
	CDevice::GetInstance()->GetCommandList()->Reset(CDevice::GetInstance()->GetCommandAllocator(), nullptr);
}

void CDevice::Close()
{
	CDevice::GetInstance()->GetCommandList()->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { CDevice::GetInstance()->GetCommandList() };
	CDevice::GetInstance()->GetCommandQueue()->ExecuteCommandLists(1, ppd3dCommandLists);
}

void CDevice::Begin()
{
	if (FAILED(CDevice::GetInstance()->GetCommandAllocator()->Reset()))
		return;
	Open();

	D3D12_RESOURCE_BARRIER	tResource_Barrier =
		CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetBuffers[m_iSwapChainBufferIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_pCommandList->ResourceBarrier(1, &tResource_Barrier);


	CD3DX12_CPU_DESCRIPTOR_HANDLE	rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_iSwapChainBufferIdx, m_iRtvDescriptorIncrementSize);

	CD3DX12_CPU_DESCRIPTOR_HANDLE	dsvHandle(m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
	float pfClearColor[4] = { 0.f, 0.f, 1.f, 1.f };
	m_pCommandList->ClearRenderTargetView(rtvHandle, pfClearColor, 0, nullptr);
	m_pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_pCommandList->RSSetViewports(1, &m_ViewPort);
	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);
}

void CDevice::End()
{
	//PIXEndEvent(m_pCommandList);
	D3D12_RESOURCE_BARRIER	tResource_Barrier =
		CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetBuffers[m_iSwapChainBufferIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_pCommandList->ResourceBarrier(1, &tResource_Barrier);


	Close();

	WaitForGpuComplete();
	m_pSwapChain->Present(1, 0);
	MoveToNextFrame();

}

ID3D12RootSignature* CDevice::GetRootSignature(ROOT_TYPE eType)
{
	return m_vecRootSignature[eType];
}

void CDevice::Free()
{
	OnDestroy();
}
