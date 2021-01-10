#include "framework.h"
#include "..\Headers\Device.h"

_IMPLEMENT_SINGLETON(CDevice)
CDevice::CDevice()
{
}

HRESULT CDevice::Initialize()
{
	UINT iFlag = 0;

#ifdef _DEBUG
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDbgCtrl));
	m_pDbgCtrl->EnableDebugLayer();
#endif

	CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));

	// CreateDevice
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));

	// CreateFence
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	m_iFenceValue = 1;

	// Create an event handle to use for frame synchronization.
	m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_hFenceEvent == nullptr)
	{
		assert(nullptr);
	}

	// Command Queue 	
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCmdQueue));

	// Create Command Allocator
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCmdAlloc));

	// Create the command list.
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_pCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListGraphic));

	m_pCmdListGraphic->Close();

	// SwapChain �����
	if (FAILED(Create_SwapChain(true)))
		return E_FAIL;

	// View �����
	if (FAILED(Create_View()))
		return E_FAIL;

	// ViewPort �����
	if (FAILED(Create_ViewPort()))
		return E_FAIL;

	// Empty Signature �����
	// ��Ʈ ���� 
	// �׸��� ȣ�� ���� �ش� �ڿ��� ���������ο� ���� �ڷ��̸�,
	// ��� ������ ���̴���, �Ǵ� ���̴� �ڿ����� ��� �� �������� ���������� ����Ѵ�.

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;

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


	D3D12SerializeRootSignature(&signature, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_ArrRootSignature[(UINT)ROOT_SIG_TYPE::RENDER]));

	return S_OK;
}

HRESULT CDevice::Create_SwapChain(_bool IsWindowed)
{
	DXGI_SWAP_CHAIN_DESC	tDesc = { };
	tDesc.BufferCount = 2; // dx12 �� ���� ī��Ʈ�� 2�� �־��ش�

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ �ȼ� ����(�ȼ��� 4����Ʈ
	tDesc.BufferDesc.Width = (UINT)WINCX;  // ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Height = (UINT)WINCY;// ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // ������� �ػ� ����ġ�� ȭ�� �����ϸ��� �Ұ�����
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �ȼ��� ��ĵ���� ����
	tDesc.BufferDesc.RefreshRate.Numerator = 100; // ȭ�� ���� ����
	tDesc.BufferDesc.RefreshRate.Denominator = 1;    // ȭ�� ���� ���� 

	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ��� Ÿ�� �뵵�� ���۸� �����.
	tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	tDesc.OutputWindow = g_hWnd;	// ��� ������
	tDesc.Windowed = IsWindowed;   // â ��� or ��üȭ�� ���
	tDesc.SampleDesc.Count = 1;		// ��Ƽ ���� ��� ����
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� �ĸ� ���� ��ü �� ���� ������ ���� ����

	if (FAILED(m_pFactory->CreateSwapChain(m_pCmdQueue.Get(), &tDesc, &m_pSwapChain)))
		return E_FAIL;

	return S_OK;
}

void CDevice::Open()
{
	m_pCmdListGraphic->Reset(m_pCmdAlloc.Get(), nullptr);
}

void CDevice::Close()
{
	m_pCmdListGraphic->Close();
	// Ŀ�ǵ� ����Ʈ ����	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListGraphic.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void CDevice::Free()
{
	WaitForFenceEvent();
	CloseHandle(m_hFenceEvent);
}

HRESULT CDevice::Create_ViewPort()
{
	m_tViewPort = D3D12_VIEWPORT{ 0.f, 0.f, WINCX, WINCY, 0.f, 1.f };
	m_tScissorRect = D3D12_RECT{ 0, 0, (LONG)WINCX, (LONG)WINCY };

	return S_OK;
}

HRESULT CDevice::Create_View()
{
	// RenderTargetView
	{
		m_iRTVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// dx12 �������� ���� RenderTargetView ����
		// Descriptor Heap(������ ��) �� Dx11 �� RTV, DSV, UAV, SRV �� ���� ��ü

		// RenderTargetView �����	
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		tDesc.NumDescriptors = 2;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		m_pDevice->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));

		D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

		// Create a RTV for each frame.
		for (UINT i = 0; i < 2; i++)
		{
			// ������ SwapChain ���� ���۸� ������
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));

			// �ش� ���۷� RenderTarvetView ������
			m_pDevice->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, hRTVHeap);
			hRTVHeap.ptr += m_iRTVHeapSize; // Offset ����
		}
	}

	// ConstantBufferView
	{
		m_iSRVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.NumDescriptors = 1;
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		tDesc.NodeMask = 0;
		if (FAILED(m_pDevice->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pCbv))))
			return E_FAIL;

		D3D12_CPU_DESCRIPTOR_HANDLE hCbvHeap = m_pCbv->GetCPUDescriptorHandleForHeapStart();

	}



	return S_OK;
}

void CDevice::Render_Begin(float(&_arrFloat)[4])
{
	m_pCmdAlloc->Reset();
	Open();

	// �ʿ��� ���� ����	
	m_pCmdListGraphic->RSSetViewports(1, &m_tViewPort);
	m_pCmdListGraphic->RSSetScissorRects(1, &m_tScissorRect);

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// ��¿���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �ٽ� ����۷� ����
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	hRTVHandle.ptr += (m_iCurTargetIdx * m_iRTVHeapSize);
	m_pCmdListGraphic->OMSetRenderTargets(1, &hRTVHandle, FALSE, nullptr);
	m_pCmdListGraphic->ClearRenderTargetView(hRTVHandle, _arrFloat, 0, nullptr);
}

void CDevice::Render_End()
{
	// Indicate that the back buffer will now be used to present.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// ����ۿ���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �ٽ� ������� ����

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);

	Close();
	// Present the frame.
	m_pSwapChain->Present(0, 0);

	WaitForFenceEvent();

	// ����� Ÿ�� �ε��� ����
	m_iCurTargetIdx == 0 ? m_iCurTargetIdx = 1 : m_iCurTargetIdx = 0;
}

void CDevice::WaitForFenceEvent()
{
	// Signal and increment the fence value.
	const UINT64 fence = m_iFenceValue;
	m_pCmdQueue->Signal(m_pFence.Get(), fence);
	m_iFenceValue++;

	UINT64 a = m_pFence->GetCompletedValue();
	// Wait until the previous frame is finished.
	if (a < fence)
	{
		m_pFence->SetEventOnCompletion(fence, m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}