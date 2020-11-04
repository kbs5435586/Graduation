#pragma once
#include "Base.h"
class CRenderer;
class CDevice :
    public CBase
{
	_DECLARE_SINGLETON(CDevice);
private:
    CDevice();
	virtual  ~CDevice() = default;
public:
	HRESULT										Initialize();
	HRESULT										OnCreate();
	HRESULT										OnDestroy();
	HRESULT										CreateSwapChain();
	HRESULT										CreateDevice();
	HRESULT										CreateDescriptorHeap();
	HRESULT										CreateCommandQueueAndList();
	HRESULT										CreateRenderTargerView();
	HRESULT										CreateDepthStencilView();
public:
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
public:
	void										WaitForGpuComplete();
	void										MoveToNextFrame();
public:
	HRESULT										CreateRootSignature(ROOT_TYPE eType);
public:
	ID3D12Device*								GetDevice() { return m_pDevice; }
	ID3D12GraphicsCommandList*					GetCommandList() { return m_pCommandList; }
	ID3D12CommandAllocator*						GetCommandAllocator() { return m_pCommandAllocator; }
	ID3D12CommandQueue*							GetCommandQueue() { return m_pCommandQueue; }
private:
	_matrix										m_matProj = {};
	_matrix										m_matView = {};
public:
	_matrix										GetProjectionMatrix() { return m_matProj; }
	_matrix										GetViewMatrix() { return m_matView; }
public:
	void										SetProjectionMatrix(_matrix matProj) { m_matProj = matProj; }
	void										SetViewMatrix(_matrix matView) { m_matView = matView; }
public:
	D3D12_VIEWPORT								GetViewPort() { return m_ViewPort; }
public:
	void										Open();
	void										Close();
public:
	void										Begin();
	void										End();
private:
	_bool										m_bMsaaEnalbe = false;
	_uint										m_iMsaaQualityLv = 0;
	static const _uint							m_iSwapchainBuffer = 2;
	_uint										m_iSwapChainBufferIdx = 0;
private:
	_uint										m_iRtvDescriptorIncrementSize = 0;
	_uint										m_iDsvDescriptorIncrementSize = 0;
	_uint										m_iCbvDescriptorIncrementSize = 0;
private:
	IDXGIFactory4*								m_pFactory = nullptr;
	IDXGISwapChain3*							m_pSwapChain = nullptr;
	ID3D12Device*								m_pDevice = nullptr;
	ID3D12CommandQueue*							m_pCommandQueue = nullptr;
	ID3D12CommandAllocator*						m_pCommandAllocator = nullptr;
	ID3D12GraphicsCommandList*					m_pCommandList = nullptr;
	ID3D12Fence*								m_pFence = nullptr;
private:
	ID3D12Resource*								m_pRenderTargetBuffers[m_iSwapchainBuffer] = { nullptr };
	ID3D12Resource*								m_pDepthStencilBuffer = nullptr;
private:
	ID3D12DescriptorHeap*						m_pRtvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap*						m_pDsvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap*						m_pCbvDescriptorHeap = nullptr;
public:
	ID3D12DescriptorHeap*						GetConstantBufferDescriptorHeap() { return m_pCbvDescriptorHeap; }
private:
	vector<ID3D12RootSignature*>				m_vecRootSignature;
private:
	HANDLE										m_hFenceEvent = {};
	_uint										m_iFenceValues[m_iSwapchainBuffer];
#if defined(_DEBUG)
	ID3D12Debug*								m_pDebugController = nullptr;
#endif	
	D3D12_VIEWPORT								m_ViewPort = {};
	D3D12_RECT									m_ScissorRect = {};
public:
	ID3D12RootSignature*					GetRootSignature(ROOT_TYPE eType);
	ID3D12RootSignature* TempRoot;
protected:
	virtual void								Free();
};

