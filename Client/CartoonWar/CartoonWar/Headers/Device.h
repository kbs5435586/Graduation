#pragma once
#include "Base.h"
class CDevice :
    public CBase
{
    _DECLARE_SINGLETON(CDevice)
private:
    CDevice();
    virtual ~CDevice() = default;
private:
	ComPtr<ID3D12Device>				m_pDevice = nullptr;
	ComPtr<ID3D12PipelineState>			m_pPipelineState = nullptr;
	ComPtr<ID3D12CommandQueue>			m_pCmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator>		m_pCmdAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList>	m_pCmdListGraphic = nullptr;
	ComPtr<ID3D12Fence>					m_pFence = nullptr;
	ComPtr<IDXGIFactory>				m_pFactory = nullptr;
	ComPtr<ID3D12Debug>					m_pDbgCtrl = nullptr;
	ComPtr<IDXGISwapChain>				m_pSwapChain = nullptr;
	ComPtr<ID3D12Resource>				m_RenderTargets[2] = {nullptr};
	ComPtr<ID3D12Resource>				m_pConstantBuffer=nullptr;

	ComPtr<ID3D12DescriptorHeap>		m_pRTV = nullptr;
	ComPtr<ID3D12DescriptorHeap>		m_pDSV = nullptr;
	ComPtr<ID3D12DescriptorHeap>		m_pCbv = nullptr;
public:
	ComPtr<ID3D12Device>				GetDevice() { return m_pDevice; }
	ComPtr<ID3D12GraphicsCommandList>	GetCmdLst() { return m_pCmdListGraphic; }
	ComPtr<ID3D12RootSignature>			GetRootSignature(ROOT_SIG_TYPE _eType) { return m_ArrRootSignature[(UINT)_eType]; }
public:
	ComPtr<ID3D12DescriptorHeap>		GetConstantBufferDescHeap() { return m_pCbv; }
public:
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
private:
	ComPtr<ID3D12RootSignature>			m_ArrRootSignature[(_uint)ROOT_SIG_TYPE::END];
private:
	D3D12_VIEWPORT						m_tViewPort = {};
	D3D12_RECT							m_tScissorRect = {};
private:
	HANDLE								m_hFenceEvent = 0;
	_uint								m_iFenceValue = 0;
	_uint								m_iCurTargetIdx=0;
	_uint								m_iRTVHeapSize=0;
	_uint								m_iSRVHeapSize = 0;
public:
	HRESULT								Initialize();
	void								Render_Begin(float(&_arrFloat)[4]);
	void								Render_End();
	void								WaitForFenceEvent();
public:
	void								Open();
	void								Close();
private:
	HRESULT								Create_SwapChain(_bool IsWindowed);
	HRESULT								Create_View();
	HRESULT								Create_ViewPort();
private:
    virtual void						Free();
};

