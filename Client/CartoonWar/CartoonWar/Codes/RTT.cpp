#include "framework.h"
#include "RTT.h"
#include "Management.h"

CRTT::CRTT()
{
}

HRESULT CRTT::Ready_RTT(const _tchar* pTag, ComPtr<ID3D12Resource> pResource)
{
	lstrcpy(m_pRtt_Tag, pTag);

	m_pTexture = pResource;
	m_tDesc = pResource->GetDesc();

	if(m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV))))
		{
			return E_FAIL;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		CDevice::GetInstance()->GetDevice()->CreateDepthStencilView(m_pTexture.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
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
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = m_Image.GetMetadata().format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_pTexture.Get(),
				&srvDesc, m_pSRV->GetCPUDescriptorHandleForHeapStart());
		}
	}
	return S_OK;
}

void CRTT::Set_RenderTarget(ID3D12DescriptorHeap* pDsv)
{

}

HRESULT CRTT::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();


	return S_OK;
}


CRTT* CRTT::Create(const _tchar* pTag, ComPtr<ID3D12Resource> pResource)
{
	CRTT* pInstance = new CRTT();
	if (FAILED(pInstance->Ready_RTT(pTag, pResource)))
		Safe_Release(pInstance);
	return pInstance;
}

void CRTT::Free()
{

}
