#include "framework.h"
#include "RTT.h"
#include "Management.h"

CRTT::CRTT()
{

}


HRESULT CRTT::CreateFromResource(const _tchar* pTag, ComPtr<ID3D12Resource> _pTex2D)
{
	lstrcpy(m_pTag, pTag);
	m_pTexture = _pTex2D;
	m_tDesc = _pTex2D->GetDesc();

	HRESULT hr = S_OK;
	if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		hr = CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV));

		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		CDevice::GetInstance()->GetDevice()->CreateDepthStencilView(m_pTexture.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
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

		if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE)
		{
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
		}
	}

	return S_OK;
}

HRESULT CRTT::Create_Texture(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat, 
	const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag, D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearColor)
{
	lstrcpy(m_pTag, pTag);
	m_tDesc.MipLevels = 1;
	m_tDesc.Format = _eFormat;
	m_tDesc.Width = _iWidth;
	m_tDesc.Height = _iHeight;
	m_tDesc.Flags = _eResFlag;
	m_tDesc.DepthOrArraySize = 1;
	m_tDesc.SampleDesc.Count = 1;
	m_tDesc.SampleDesc.Quality = 0;
	m_tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_tDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;


	D3D12_CLEAR_VALUE* pValue = nullptr;
	m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (_eResFlag & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
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
	{
		return E_FAIL;
	}
	

	// Texture 를 관리할 View 생성(SRV, RTV, DSV)
	if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV))))
		{
			CDevice::GetInstance()->Close();
			CDevice::GetInstance()->WaitForFenceEvent();
			return E_FAIL;
		}

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

CRTT* CRTT::Create(const _tchar* pTag, ComPtr<ID3D12Resource> _pTex2D)
{
	CRTT* pInstance = new CRTT;
	if (FAILED(pInstance->CreateFromResource(pTag, _pTex2D)))
		Safe_Release(pInstance);

	return pInstance;
}


CRTT* CRTT::Create(const _tchar* pTag, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat, const D3D12_HEAP_PROPERTIES& _HeapProperty,
					D3D12_HEAP_FLAGS _eHeapFlag, D3D12_RESOURCE_FLAGS _eResFlag, _vec4 _vClearClolr)
{
	CRTT* pInstance = new CRTT;
	if (FAILED(pInstance->Create_Texture(pTag, _iWidth, _iHeight, _eFormat, _HeapProperty, _eHeapFlag, _eResFlag, _vClearClolr)))
		Safe_Release(pInstance);

	return pInstance;

}


void CRTT::Free()
{
}
