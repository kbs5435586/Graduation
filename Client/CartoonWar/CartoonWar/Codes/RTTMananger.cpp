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


	tRtt  arrRT[2] = {};
	for (_uint i = 0; i < 2; ++i)
	{
		wsprintf(szName, L"SwapchainTargetTex_%d", i);
		ComPtr<ID3D12Resource>		pTarget;
		CDevice::GetInstance()->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&pTarget));
		arrRT[i].pRtt = CRTT::Create(szName, pTarget);
	}

	CRTT* pDsTex = CRTT::Create(L"DepthStencilTex"
		, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_D32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
		, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);



	// 여기서 MRT를 생성해야함
	CMRT* pMRT = CMRT::Create(2, arrRT, pDsTex);
	m_vecMRT.push_back(pMRT);


	// Defferd MRT
	{
		tRtt arrRT[8] = {};
		arrRT[0].vClear_Color = {0.f,0.f,0.f,0.f};
		arrRT[0].pRtt= CRTT::Create(L"DiffuseTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);

		arrRT[1].vClear_Color = { 0.f,0.f,0.f,0.f };
		arrRT[1].pRtt = CRTT::Create(L"NormalTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[1].vClear_Color);

		arrRT[2].vClear_Color = { 0.f,0.f,0.f,0.f };
		arrRT[2].pRtt = CRTT::Create(L"PositionTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[2].vClear_Color);

		CMRT* pMRT = CMRT::Create(3, arrRT, pDsTex);
		m_vecMRT.push_back(pMRT);
	}

	// Light MRT
	{
		tRtt arrRT[8] = {};
		arrRT[0].vClear_Color = { 0.f,0.f,0.f,0.f };
		arrRT[0].pRtt = CRTT::Create(L"DiffuseLightTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClear_Color);

		arrRT[1].vClear_Color = { 0.f,0.f,0.f,0.f };
		arrRT[1].pRtt = CRTT::Create(L"SpecularLightTargetTex"
			, (UINT)WINCX, (UINT)WINCY, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[1].vClear_Color);

		CMRT* pMRT = CMRT::Create(2, arrRT, pDsTex);
		m_vecMRT.push_back(pMRT);

	}





	return S_OK;
}

void CRTTMananger::Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv)
{

}

CRTT* CRTTMananger::Get_RTT(const _tchar* pRTT_Tag)
{
	return nullptr;
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
}
