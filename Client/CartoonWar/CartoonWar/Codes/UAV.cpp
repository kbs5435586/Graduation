#include "framework.h"
#include "UAV.h"



CUAV::CUAV()
{
}

HRESULT CUAV::Ready_UAV( UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
	, const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
	, D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearColor)
{
	m_tDesc.MipLevels = 1;
	m_tDesc.Format = _eFormat;
	m_tDesc.Width = _iWidth;
	m_tDesc.Height = _iHeight;
	m_tDesc.Flags = _eResFlag;
	m_tDesc.DepthOrArraySize = 1;
	m_tDesc.SampleDesc.Count = 1;
	m_tDesc.SampleDesc.Quality = 0;
	m_tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_tDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_tDesc.SampleDesc.Count = 1;
	m_tDesc.SampleDesc.Quality = 0;
	m_tDesc.Alignment = 0;
	m_tDesc.DepthOrArraySize = 1;
	m_tDesc.MipLevels = 1;

	D3D12_CLEAR_VALUE* pValue = nullptr;
	m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (_eResFlag & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0);
		pValue = &depthOptimizedClearValue;
		m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}
	else if (_eResFlag & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
		float arrFloat[4] = { _vClearColor.x, _vClearColor.y, _vClearColor.z, _vClearColor.w };
		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(_eFormat, arrFloat);
		pValue = &depthOptimizedClearValue;
	}
	if (FAILED(CDevice::GetInstance()->GetDevice()->
		CreateCommittedResource(&_HeapProperty, _eHeapFlag, &m_tDesc, m_eState, pValue, IID_PPV_ARGS(&m_pTexture))))
		return E_FAIL;

	// Texture 를 관리할 View 생성(SRV, RTV, DSV)
	if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV))))
			return E_FAIL;

		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		CDevice::GetInstance()->GetDevice()->CreateDepthStencilView(m_pTexture.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			// RenderTargetView 만들기	
			D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
			tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			tDesc.NumDescriptors = 1;
			tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			tDesc.NodeMask = 0;
			CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));
			D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

			CDevice::GetInstance()->GetDevice()->CreateRenderTargetView(m_pTexture.Get(), nullptr, hRTVHeap);
		}

		else if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
			uavHeapDesc.NumDescriptors = 1;
			uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_pUAV));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pUAV->GetCPUDescriptorHandleForHeapStart();

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = _eFormat;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			CDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(m_pTexture.Get(), nullptr, &uavDesc, handle);
		}

		// SRV 를 저장할 DescriptorHeap Create
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc.Format = m_Image.GetMetadata().format;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pSRV->GetCPUDescriptorHandleForHeapStart());
	}

	return S_OK;
}

CUAV* CUAV::Create( UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
	, const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
	, D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClol)
{
	CUAV* pInstance = new CUAV();
	if (FAILED(pInstance->Ready_UAV( _iWidth, _iHeight, _eFormat, _HeapProperty, _eHeapFlag, _eResFlag, _vClearClol)))
		Safe_Release(pInstance);

	return pInstance;
}

void CUAV::Dispatch(_uint x, _uint y, _uint z)
{

	CDevice::GetInstance()->UpdateTable_CS();
	CDevice::GetInstance()->GetCsCmdLst()->Dispatch(x, y, z);
	CDevice::GetInstance()->ExcuteComputeShader();
	CDevice::GetInstance()->ClearDummyDesc_CS();
}

void CUAV::Free()
{
}
