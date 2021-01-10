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

	// SwapChain 만들기
	if (FAILED(Create_SwapChain(true)))
		return E_FAIL;

	// View 만들기
	if (FAILED(Create_View()))
		return E_FAIL;

	// ViewPort 만들기
	if (FAILED(Create_ViewPort()))
		return E_FAIL;

	// Empty Signature 만들기
	// 루트 서명 
	// 그리기 호출 전에 해당 자원이 파이프라인에 묶일 자료이며,
	// 어느 시점에 묶이는지, 또는 쉐이더 자원으로 사용 시 레지스터 대응정보를 기술한다.

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
	tDesc.BufferCount = 2; // dx12 는 버퍼 카운트는 2를 넣어준다

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 픽셀 포멧(픽셀당 4바이트
	tDesc.BufferDesc.Width = (UINT)WINCX;  // 버퍼의 해상도(윈도우 해상도랑 일치시켜놓음)
	tDesc.BufferDesc.Height = (UINT)WINCY;// 버퍼의 해상도(윈도우 해상도랑 일치시켜놓음)
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // 윈도우와 해상도 불일치시 화면 스케일링을 할것인지
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 픽셀의 스캔라인 순서
	tDesc.BufferDesc.RefreshRate.Numerator = 100; // 화면 갱신 비율
	tDesc.BufferDesc.RefreshRate.Denominator = 1;    // 화면 갱신 비율 

	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 출력 타겟 용도로 버퍼를 만든다.
	tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	tDesc.OutputWindow = g_hWnd;	// 출력 윈도우
	tDesc.Windowed = IsWindowed;   // 창 모드 or 전체화면 모드
	tDesc.SampleDesc.Count = 1;		// 멀티 샘플 사용 안함
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림

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
	// 커맨드 리스트 수행	
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

		// dx12 설명자힙 으로 RenderTargetView 생성
		// Descriptor Heap(설명자 힙) 이 Dx11 의 RTV, DSV, UAV, SRV 를 전부 대체

		// RenderTargetView 만들기	
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
			// 생성된 SwapChain 에서 버퍼를 가져옴
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));

			// 해당 버퍼로 RenderTarvetView 생성함
			m_pDevice->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, hRTVHeap);
			hRTVHeap.ptr += m_iRTVHeapSize; // Offset 증가
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

	// 필요한 상태 설정	
	m_pCmdListGraphic->RSSetViewports(1, &m_tViewPort);
	m_pCmdListGraphic->RSSetScissorRects(1, &m_tScissorRect);

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 출력에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 다시 백버퍼로 지정
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
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 백버퍼에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 다시 출력으로 지정

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);

	Close();
	// Present the frame.
	m_pSwapChain->Present(0, 0);

	WaitForFenceEvent();

	// 백버퍼 타겟 인덱스 변경
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