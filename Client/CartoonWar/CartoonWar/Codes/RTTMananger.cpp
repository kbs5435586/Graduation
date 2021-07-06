#include "framework.h"
#include "RTTMananger.h"
#include "MRT.h"

_IMPLEMENT_SINGLETON(CRTTMananger)

CRTTMananger::CRTTMananger()
{

}

HRESULT CRTTMananger::Ready_RTTMananger()
{
	m_iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_tchar szName[128] = {};


	// Create RenderTarget for SwapChain
	tRtt  arrRT[2] = {};
	for (_uint i = 0; i < 2; ++i)
	{
		wsprintf(szName, L"SwapchainTargetTex_%d", i);
		ComPtr<ID3D12Resource>		pTarget;
		CDevice::GetInstance()->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&pTarget));
		arrRT[i].pRtt = CRTT::Create(szName, pTarget);
		arrRT[i].vClear_Color = {0.f,0.f,1.f,1.f};
		if (arrRT[i].pRtt == nullptr)
			return E_FAIL;
	}

	m_pDsTex = CRTT::Create(L"DepthStencilTex"
		, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_D24_UNORM_S8_UINT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
		, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	if (m_pDsTex == nullptr)
		return E_FAIL;

	//DXGI_FORMAT_D24_UNORM_S8_UINT

	// SwapChain MRT
	CMRT* pMRT = CMRT::Create(2, arrRT, m_pDsTex);
	m_vecMRT.push_back(pMRT);


	// Defferd MRT
	{
		tRtt arrRT[3] = {};
		arrRT[0].vClear_Color = { 0.f,0.f,0.f,1.f };
		arrRT[0].pRtt= CRTT::Create(L"DiffuseTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);


		arrRT[1].vClear_Color = { 0.f,0.f,0.f,1.f };
		arrRT[1].pRtt = CRTT::Create(L"NormalTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R32G32B32A32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[1].vClear_Color);

		
		arrRT[2].vClear_Color = { 0.f,0.f,0.f,1.f };
		arrRT[2].pRtt = CRTT::Create(L"PositionTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R32G32B32A32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[2].vClear_Color);



		CMRT* pMRT = CMRT::Create(3, arrRT, m_pDsTex);
		m_vecMRT.push_back(pMRT);

	}

	// Light MRT
	{
		tRtt arrRT[2] = {};
		arrRT[0].vClear_Color = { 0.f,0.f,0.f,1.f };
		arrRT[0].pRtt = CRTT::Create(L"DiffuseLightTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);



		arrRT[1].vClear_Color = { 0.f,0.f,0.f,1.f };
		arrRT[1].pRtt = CRTT::Create(L"SpecularLightTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[1].vClear_Color);

		CMRT* pMRT = CMRT::Create(2, arrRT, m_pDsTex);
		m_vecMRT.push_back(pMRT);

	}


	// Shadow MRT
	{
		tRtt arrRT[1] = {};
		arrRT[0].vClear_Color = { 0.f,0.f,0.f,0.f };
		arrRT[0].pRtt = CRTT::Create(L"ShadowMapTargetTex"
			, 4096, 4096
			, DXGI_FORMAT_R32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);

		CRTT* pDsTex = CRTT::Create(L"ShadowDepathTex"
			, 4096, 4096, DXGI_FORMAT_D24_UNORM_S8_UINT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		if (pDsTex == nullptr)
			return E_FAIL;

		CMRT* pMRT = CMRT::Create(1, arrRT, pDsTex);
		m_vecMRT.push_back(pMRT);
	}

	// PostEffectTex
	{

		m_pPostEffectTex = CRTT::Create(L"PostEffectTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_NONE, arrRT[0].vClear_Color);
		
		if (nullptr == m_pPostEffectTex)
			return E_FAIL;
	}

	// Blur Tex
	{
		tRtt arrRT[1] = {};
		arrRT[0].vClear_Color = { 0.f,0.f,0.f,1.f };

		arrRT[0].pRtt = CRTT::Create(L"BlurTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R32G32B32A32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);

		if (nullptr == arrRT[0].pRtt)
			return E_FAIL;

		CMRT* pMRT = CMRT::Create(1, arrRT, m_pDsTex);
		m_vecMRT.push_back(pMRT);
	}


	return S_OK;
}

void CRTTMananger::Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv)
{

}


CRTTMananger* CRTTMananger::Create()
{
	CRTTMananger* pInstance = new CRTTMananger();
	if (FAILED(pInstance->Ready_RTTMananger()))
		Safe_Release(pInstance);

	return pInstance;
}

void CRTTMananger::Free()
{
	for (auto& iter : m_vecMRT)
	{
		Safe_Release(iter);
	}
	Safe_Release(m_pDsTex);
	Safe_Release(m_pPostEffectTex);
}
