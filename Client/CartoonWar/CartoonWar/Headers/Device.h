#pragma once
#include "Base.h"
class CTexture;
class CDevice :
    public CBase
{
    _DECLARE_SINGLETON(CDevice)
private:
    CDevice();
    virtual ~CDevice() = default;
private:
	ComPtr<ID3D12Device>						m_pDevice = nullptr;
	ComPtr<ID3D12PipelineState>					m_pPipelineState = nullptr;
	ComPtr<ID3D12CommandQueue>					m_pCmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator>				m_pCmdAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList>			m_pCmdListGraphic = nullptr;
	ComPtr<ID3D12Fence>							m_pFence = nullptr;
	ComPtr<IDXGIFactory4>						m_pFactory = nullptr;
	ComPtr<ID3D12Debug>							m_pDbgCtrl = nullptr;
	ComPtr<IDXGISwapChain4>						m_pSwapChain = nullptr;
	ComPtr<ID3D12Resource>						m_RenderTargets[2] = {nullptr};
private:
	ComPtr<ID3D12Resource>						m_pConstantBuffer=nullptr;
	ComPtr<ID3D12Resource>						m_pDSBuffer = nullptr;
private:
	vector<ComPtr<ID3D12DescriptorHeap>>		m_vecDummyDescriptor;
	vector<D3D12_STATIC_SAMPLER_DESC>			m_vecSamplerDesc;
private:
	ComPtr<ID3D12DescriptorHeap>				m_pRTV = nullptr;
	ComPtr<ID3D12DescriptorHeap>				m_pDSV = nullptr;
	ComPtr<ID3D12DescriptorHeap>				m_pInitDescriptor = nullptr;
public:
	ComPtr<ID3D12Device>						GetDevice() { return m_pDevice; }
	ComPtr<ID3D12GraphicsCommandList>			GetCmdLst() { return m_pCmdListGraphic; }
	ComPtr<ID3D12CommandQueue>					GetCmdQueue() { return m_pCmdQueue; }
	ComPtr<ID3D12RootSignature>					GetRootSignature(ROOT_SIG_TYPE _eType) { return m_ArrRootSignature[(UINT)_eType]; }
	vector<ComPtr<ID3D12DescriptorHeap>>		GetDummyDesc() { return m_vecDummyDescriptor; }
	ComPtr<ID3D12Resource>						GetRenderTarget() { return m_RenderTargets[m_iCurTargetIdx]; }
	ComPtr< ID3D12DescriptorHeap>				GetDSV() { return m_pDSV; }
	ComPtr<IDXGISwapChain4>						GetSwapChain() { return m_pSwapChain; }

public:
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
public:
	_uint&										GetHDRMetaDataIdx() { return m_iHDRMetaDataPoolIdx; }
private:
	ComPtr<ID3D12RootSignature>					m_ArrRootSignature[(_uint)ROOT_SIG_TYPE::END];
private:
	D3D12_VIEWPORT								m_tViewPort = {};
	D3D12_RECT									m_tScissorRect = {};
private:
	DXGI_COLOR_SPACE_TYPE						m_CurrnetSwapChainColorSpace;
	SwapChainBitDepth							m_CurrentSwapChainBitDepth;
	DXGI_FORMAT									m_swapChainFormats[(_uint)SwapChainBitDepth::SwapChainBitDepthCount];
public:
	DXGI_FORMAT									GetSwapChainFormat(_int iIdx) { return m_swapChainFormats[iIdx]; }
	_int										GetBitDepth() { return (_int)m_CurrentSwapChainBitDepth; }
	_uint										GetSwapChainIdx() { return m_iCurTargetIdx; }
private:
	HANDLE										m_hFenceEvent = 0;
	_uint										m_iFenceValue = 0;
	_uint										m_iCurTargetIdx=0;
private:
	_uint										m_iRTVHeapSize=0;
	_uint										m_iSRVHeapSize = 0;
	_uint										m_iDSVHeapSize = 0;
	_uint										m_iHDRMetaDataPoolIdx = 0;;
	_uint										m_iCurrentDummyIdx = 0;
private:
	_uint										m_iRootConstant[(_uint)RootConstants::RootConstantsCount];
private:
	_bool										m_IsHDRSupport= false;
	_bool										m_IsEnableST2084 = false;
	_bool										m_IsTearingSupport = true;
private:
	_uint										m_iFactoryFlags=0;
public:
	static _float								m_fHDRMetaDataPool[4][4];
public:
	HRESULT										Initialize();
	void										Render_Begin();
	void										Render_End();
	void										WaitForFenceEvent();
public:
	void										Open();
	void										Close();
private:
	HRESULT										Create_SwapChain(_bool IsWindowed);
	HRESULT										Create_View();
	HRESULT										Create_ViewPort();
	HRESULT										Create_RootSignature();
	HRESULT										Create_SamplerDesc();
private:
	HRESULT										CheckHDRSupport();
	HRESULT										CheckSupportTearing();
	_int										ComputeIntersectionArea(_int ax1, _int ay1, _int ax2, _int ay2,
																		_int bx1, _int by1, _int bx2, _int by2);
	HRESULT										EnsureSwapChainColorSpace(SwapChainBitDepth swapChainBitDepth, _bool enableST);
public:
	HRESULT										SetHDRMetaData(_float fMaxOutputNits, _float fMinOutputNits, _float fMaxCLL, _float fMaxFall);
public:
	void										SetTextureToShader(ID3D12DescriptorHeap* pTextureDesc, TEXTURE_REGISTER eRegisterNum);
	void										SetConstantBufferToShader(ID3D12DescriptorHeap* pConstantBuffer, _uint iOffset, CONST_REGISTER eRegisterNum);
	void										SetGlobalConstantBufferToShader(ID3D12DescriptorHeap* pConstantBuffer, _uint iOffset, CONST_REGISTER eRegisterNum);

	void										UpdateTable();
private:
	void										ClearDummyDesc(_uint iIdx);
private:
    virtual void								Free();
};

