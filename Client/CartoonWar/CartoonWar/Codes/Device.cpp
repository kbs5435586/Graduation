#include "framework.h"
#include "..\Headers\Device.h"
#include "Texture.h"
#include "Management.h"
#include "MRT.h"
#include "RTT.h"
#include "Texture.h"
#include "UAV.h"
#include "StructedBuffer.h"

_IMPLEMENT_SINGLETON(CDevice)
CDevice::CDevice()
{
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
_float CDevice::m_fHDRMetaDataPool[4][4] =
{
	{ 1000.0f, 0.001f, 2000.0f, 500.0f },

	{ 500.0f, 0.001f, 2000.0f, 500.0f },

	{ 500.0f, 0.100f, 500.0f, 100.0f },

	{ 2000.0f, 1.000f, 2000.0f, 1000.0f }
};

HRESULT CDevice::Initialize()
{
	UINT iFlag = 0;

#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDbgCtrl))))
	{
		m_pDbgCtrl->EnableDebugLayer();
		m_iFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

#endif
	CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));

	// CreateDevice
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));

	// CreateFence
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFenceCS));
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

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCsCmdQueue));

	// Create Command Allocator
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCmdAlloc));
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pResCmdAlloc));
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pCsCmdAlloc));

	// Create the command list.
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListGraphic));
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pResCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pResCmdList));
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pCsCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pCsCmdList));


	// SwapChain 만들기
	if (FAILED(Create_SwapChain(true)))
		return E_FAIL;

	//if (FAILED(Create_ViewPort()))
	//	return E_FAIL;
	if (FAILED(Create_RootSignature()))
		return E_FAIL;

	
	m_pCmdListGraphic->Close();
	//m_pResCmdList->Close();
	//m_pCsCmdList->Close();


	return S_OK;
	
}

HRESULT CDevice::Create_SwapChain(_bool IsWindowed)
{
	DXGI_SWAP_CHAIN_DESC tDesc = {};

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

	HRESULT hr = m_pFactory->CreateSwapChain(m_pCmdQueue.Get(), &tDesc, &m_pSwapChain);
	return S_OK;
}

void CDevice::Open()
{
	m_pCmdAlloc->Reset();
	m_pCmdListGraphic->Reset(m_pCmdAlloc.Get(), nullptr);
}

void CDevice::Close()
{
	m_pCmdListGraphic->Close();
	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListGraphic.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void CDevice::ResCmdOpen()
{
	m_pResCmdAlloc->Reset();
	m_pResCmdList->Reset(m_pResCmdAlloc.Get(), nullptr);
}

void CDevice::ResCmdClose()
{
	m_pResCmdList->Close();
	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pResCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	WaitForFenceEvent();
}

void CDevice::CsCmdOpen()
{
	m_pCsCmdAlloc->Reset();
	m_pCsCmdList->Reset(m_pCsCmdAlloc.Get(), nullptr);
}

void CDevice::CsCmdClose()
{
	m_pCsCmdList->Close();
	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCsCmdList.Get() };
	m_pCsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	WaitForFenceEvent_CS();
}


void CDevice::SetTextureToShader(CTexture* pTextureCom,  TEXTURE_REGISTER eRegisterNum, const _uint& iIdx)
{
	if (m_iCurrentDummyIdx >= 1024)
		return;

	_uint			iDestRange = 1;
	_uint			iSrcRange = 1;

	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE	hDestHandle = m_vecDummyDescriptor[m_iCurrentDummyIdx]->GetCPUDescriptorHandleForHeapStart();

	hDestHandle.ptr += (iSize* (_uint)eRegisterNum);

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pTextureCom->GetSRV(iIdx)->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDestHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//if (pTextureCom->GetState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	//{
	//	CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(pTextureCom->GetTexture(iIdx)
	//		, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	//	m_pCsCmdList->ResourceBarrier(1, &temp);
	//	pTextureCom->SetState(D3D12_RESOURCE_STATE_COMMON);
	//}

}

void CDevice::SetTextureToShader(ID3D12DescriptorHeap* pTextureDesc, TEXTURE_REGISTER eRegisterNum)
{
	if (m_iCurrentDummyIdx >= 1024)
		return;

	if (nullptr == pTextureDesc)
		return;
	_uint			iDestRange = 1;
	_uint			iSrcRange = 1;

	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE	hDestHandle = m_vecDummyDescriptor[m_iCurrentDummyIdx]->GetCPUDescriptorHandleForHeapStart();

	hDestHandle.ptr += (iSize * (_uint)eRegisterNum);

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pTextureDesc->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDestHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetConstantBufferToShader(ID3D12DescriptorHeap* pConstantBuffer, _uint iOffset, CONST_REGISTER eRegisterNum)
{
	if (m_iCurrentDummyIdx >= 1024)
		return;

	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE hDestHandle = m_vecDummyDescriptor[m_iCurrentDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDestHandle.ptr += iSize * (UINT)eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pConstantBuffer->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += iOffset * iSize;

	m_pDevice->CopyDescriptors(1, &hDestHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetGlobalConstantBufferToShader(ID3D12DescriptorHeap* pConstantBuffer, _uint iOffset, CONST_REGISTER eRegisterNum)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE hDestHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hDestHandle.ptr += iSize * (UINT)eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pConstantBuffer->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += iOffset * iSize;

	m_pDevice->CopyDescriptors(1, &hDestHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetUpContantBufferToShader_CS(ID3D12DescriptorHeap* pConstantBuffer, _uint iOffset, CONST_REGISTER eRegisterNum)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegisterNum;


	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pConstantBuffer->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += iOffset * iSize;

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetTextureToShader_CS(CUAV* pUAV, TEXTURE_REGISTER eRegisterNum, const _uint& iIdx)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pUAV->GetUAV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetTextureToShader_CS(ID3D12DescriptorHeap* pTextureDesc, TEXTURE_REGISTER eRegisterNum)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pTextureDesc->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetUpUAVToRegister(CUAV* pUAV, UAV_REGISTER eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pUAV->GetUAV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//// 리소스 상태 변경
	if (pUAV->GetState() == D3D12_RESOURCE_STATE_COMMON)
	{
		CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(pUAV->GetTexture().Get()
			, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_pCsCmdList->ResourceBarrier(1, &temp);
	
		pUAV->SetState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

void CDevice::SetUpUAVToRegister_CS(CStructedBuffer* pBuffer, UAV_REGISTER eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pBuffer->GetUAV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//// 리소스 상태 변경
	if (pBuffer->GetState() == D3D12_RESOURCE_STATE_COMMON)
	{
		CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(pBuffer->GetBuffer().Get()
			, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_pCsCmdList->ResourceBarrier(1, &temp);

		pBuffer->SetState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

void CDevice::SetBufferToRegister(CStructedBuffer* pBuffer, TEXTURE_REGISTER eRegister)
{
	if (m_iCurrentDummyIdx >= 1024)
		return;

	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurrentDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pBuffer->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); 

	// 리소스 상태 변경
	if (pBuffer->GetState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(pBuffer->GetBuffer().Get()
			, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

		m_pCmdListGraphic->ResourceBarrier(1, &temp);
		pBuffer->SetState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::SetBufferToRegister_CS(CStructedBuffer* pBuffer, TEXTURE_REGISTER eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;
	_uint iSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += iSize * (UINT)eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pBuffer->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 리소스 상태 변경
	if (pBuffer->GetState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(pBuffer->GetBuffer().Get()
			, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
		m_pCmdListGraphic->ResourceBarrier(1, &temp);
		pBuffer->SetState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::UpdateTable()
{
	if (m_iCurrentDummyIdx >= 1024)
		return;

	ID3D12DescriptorHeap* pDestriptor = m_vecDummyDescriptor[m_iCurrentDummyIdx].Get();
	m_pCmdListGraphic->SetDescriptorHeaps(1, &pDestriptor);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = pDestriptor->GetGPUDescriptorHandleForHeapStart();
	m_pCmdListGraphic->SetGraphicsRootDescriptorTable(0, gpuhandle);

	++m_iCurrentDummyIdx;
	ClearDummyDesc(m_iCurrentDummyIdx);
}

void CDevice::UpdateTable_CS()
{
	ID3D12DescriptorHeap* pDescriptor = m_pCsDummyDesciptor.Get();
	m_pCsCmdList->SetDescriptorHeaps(1, &pDescriptor);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = pDescriptor->GetGPUDescriptorHandleForHeapStart();
	m_pCsCmdList->SetComputeRootDescriptorTable(0, gpuhandle);
}

void CDevice::ClearDummyDesc(_uint iIdx)
{
	if (iIdx >= 1024)
		return;

	D3D12_CPU_DESCRIPTOR_HANDLE hDestHandle = m_vecDummyDescriptor[iIdx]->GetCPUDescriptorHandleForHeapStart();
	hDestHandle.ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr;

	UINT iDestRange = (UINT)TEXTURE_REGISTER::END;
	UINT iSrcRange = (UINT)TEXTURE_REGISTER::END;

	m_pDevice->CopyDescriptors(1, &hDestHandle, &iDestRange, 1
		, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void CDevice::ClearDummyDesc_CS()
{
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pCsDummyDesciptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr;

	UINT iDestRange = (UINT)TEXTURE_REGISTER::END;
	UINT iSrcRange = (UINT)TEXTURE_REGISTER::END;

	m_pDevice->CopyDescriptors(1/*디스크립터 개수*/
		, &hDescHandle, &iDestRange
		, 1/*디스크립터 개수*/
		, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::Free()
{
	WaitForFenceEvent();
	WaitForFenceEvent_CS();
	CloseHandle(m_hFenceEvent);
}

HRESULT CDevice::Create_ViewPort()
{
	m_tViewPort = D3D12_VIEWPORT{ 0.f, 0.f, WINCX, WINCY, 0.f, 1.f };
	m_tScissorRect = D3D12_RECT{ 0, 0, (LONG)WINCX, (LONG)WINCY };

	return S_OK;
}
 
HRESULT CDevice::Create_RootSignature()
{
	vector< D3D12_DESCRIPTOR_RANGE> vecRange;

	D3D12_ROOT_PARAMETER slotParam = {};
	vecRange.clear();

	D3D12_DESCRIPTOR_RANGE range = {};

	range.BaseShaderRegister = 0;
	range.NumDescriptors = (UINT)CONST_REGISTER::END;
	range.OffsetInDescriptorsFromTableStart = -1;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	vecRange.push_back(range);

	range = {};
	range.BaseShaderRegister = 0;  // t0 에서
	range.NumDescriptors = 13;	   // t12 까지 13 개 텍스쳐 레지스터 사용여부 
	range.OffsetInDescriptorsFromTableStart = (UINT)CONST_REGISTER::END;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	vecRange.push_back(range);

	slotParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	slotParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	slotParam.DescriptorTable.NumDescriptorRanges = vecRange.size();
	slotParam.DescriptorTable.pDescriptorRanges = &vecRange[0];

	// Sampler Desc 만들기
	if (FAILED(Create_SamplerDesc()))
		return E_FAIL;

	// 루트 서명 	
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 1;
	sigDesc.pParameters = &slotParam;
	sigDesc.NumStaticSamplers = 2;
	sigDesc.pStaticSamplers = &m_vecSamplerDesc[0];
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; 

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	HRESULT hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_ArrRootSignature[(UINT)ROOT_SIG_TYPE::RENDER]));

	// 더미용 Descriptor Heap 만들기
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

	UINT iDescriptorNum = 0;
	for (size_t i = 0; i < vecRange.size(); ++i)
	{
		iDescriptorNum += vecRange[i].NumDescriptors;
	}

	cbvHeapDesc.NumDescriptors = iDescriptorNum;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	for (size_t i = 0; i < 1024; ++i)
	{
		ComPtr<ID3D12DescriptorHeap> pDummyDescriptor;
		m_pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&pDummyDescriptor));
		m_vecDummyDescriptor.push_back(pDummyDescriptor);
	}

	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_pInitDescriptor));


	// Create ComputeShader RootSignature
	range = {};
	range.BaseShaderRegister = 0;  // u0 에서
	range.NumDescriptors = 4;	   // u3 까지 4 개 UAV 레지스터 사용여부 
	range.OffsetInDescriptorsFromTableStart = (UINT)TEXTURE_REGISTER::END;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	vecRange.push_back(range);


	slotParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	slotParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	slotParam.DescriptorTable.NumDescriptorRanges = (UINT)vecRange.size();
	slotParam.DescriptorTable.pDescriptorRanges = &vecRange[0];


	sigDesc = {};
	sigDesc.NumParameters = 1;
	sigDesc.pParameters = &slotParam;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pStaticSamplers = nullptr; 
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	pSignature = nullptr;
	pError = nullptr;

	hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize()
		, IID_PPV_ARGS(&m_ArrRootSignature[(UINT)ROOT_SIG_TYPE::COMPUTE]));


	m_pCsCmdList->SetComputeRootSignature(m_ArrRootSignature[(UINT)ROOT_SIG_TYPE::COMPUTE].Get());

	cbvHeapDesc.NumDescriptors += range.NumDescriptors; // UAV 만큼 추가
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_pCsDummyDesciptor));
	return S_OK;
}

HRESULT CDevice::Create_SamplerDesc()
{
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_ANISOTROPIC;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	m_vecSamplerDesc.push_back(sampler);


	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 1;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	m_vecSamplerDesc.push_back(sampler);



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

	// DepthStencilView
	{
		m_iDSVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		D3D12_DESCRIPTOR_HEAP_DESC tHeap_Desc;
		tHeap_Desc.NumDescriptors = 1;
		tHeap_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tHeap_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tHeap_Desc.NodeMask = 0;
		if(FAILED(m_pDevice->CreateDescriptorHeap(&tHeap_Desc, IID_PPV_ARGS(&m_pDSV))))
			return E_FAIL;


		D3D12_RESOURCE_DESC tDesc;
		tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tDesc.Alignment = 0;
		tDesc.Width = WINCX;
		tDesc.Height = WINCX;
		tDesc.DepthOrArraySize = 1;
		tDesc.MipLevels = 1;
		tDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality =  0;
		tDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		tDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_HEAP_PROPERTIES tHeapProperties;
		ZeroMemory(&tHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		tHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		tHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		tHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		tHeapProperties.CreationNodeMask = 1;
		tHeapProperties.VisibleNodeMask = 1;

		D3D12_CLEAR_VALUE tClearView;
		tClearView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tClearView.DepthStencil.Depth = 1.0f;
		tClearView.DepthStencil.Stencil = 0;


		if(FAILED(m_pDevice -> CreateCommittedResource(&tHeapProperties, D3D12_HEAP_FLAG_NONE, &tDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&tClearView, IID_PPV_ARGS(&m_pDSBuffer))))
			return E_FAIL;

		D3D12_CPU_DESCRIPTOR_HANDLE hDsvHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		m_pDevice->CreateDepthStencilView(m_pDSBuffer.Get(), nullptr, hDsvHandle);
	}
	


	return S_OK;
}

void CDevice::Render_Begin()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;

	m_iCurrentDummyIdx = 0;

	Open();


	// 필요한 상태 설정	
	//m_pCmdListGraphic->RSSetViewports(1, &m_tViewPort);
	//m_pCmdListGraphic->RSSetScissorRects(1, &m_tScissorRect);

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	//barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();

	barrier.Transition.pResource = pManagement->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(m_iCurTargetIdx)->pRtt->GetTex2D().Get();

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 출력에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 다시 백버퍼로 지정
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);

	ClearDummyDesc(0);
}

void CDevice::Render_End()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = pManagement->Get_RTT(
		(_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(m_iCurTargetIdx)->pRtt->GetTex2D().Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 백버퍼에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 다시 출력으로 지정
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
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

void CDevice::WaitForFenceEvent_CS()
{
	static int iFenceValue = 0;

	const size_t fence = iFenceValue;
	m_pCsCmdQueue->Signal(m_pFenceCS.Get(), fence);
	iFenceValue++;

	size_t a = m_pFenceCS->GetCompletedValue();
	// Wait until the previous frame is finished.
	if (a < fence)
	{
		m_pFenceCS->SetEventOnCompletion(fence, m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CDevice::ExcuteComputeShader()
{
	m_pCsCmdList->Close();

	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCsCmdList.Get() };
	m_pCsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitForFenceEvent_CS();

	// 다시 활성화
	m_pCsCmdAlloc->Reset();
	m_pCsCmdList->Reset(m_pCsCmdAlloc.Get(), nullptr);

	CDevice::GetInstance()->GetCsCmdLst()->SetComputeRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::COMPUTE).Get());
}
