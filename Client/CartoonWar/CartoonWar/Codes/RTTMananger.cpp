#include "framework.h"
#include "RTTMananger.h"

_IMPLEMENT_SINGLETON(CRTTMananger)

CRTTMananger::CRTTMananger()
{

}

HRESULT CRTTMananger::Ready_RTTMananger(const _tchar* pRTT_Tag)
{
	m_iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	_tchar szName[128] = {};
	vector<CRTT*>	vecRtt = {};
	for (_uint i = 0; i < 2; ++i)
	{
		wsprintf(szName, L"SwapchainTargetTex_%d", i);
		ComPtr<ID3D12Resource>		pTarget;
		CDevice::GetInstance()->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&pTarget));
		CRTT* pTemp = Create_TextureFromResource(szName, pTarget.Get());
		vecRtt.push_back(pTemp);
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

CRTTMananger* CRTTMananger::Create(const _tchar* pRTT_Tag, _uint iTextureWidth, _uint iTextureHeight)
{
	CRTTMananger* pInstance = new CRTTMananger();
	if (FAILED(pInstance->Ready_RTTMananger(pRTT_Tag)))
		Safe_Release(pInstance);

	return pInstance;
}

CRTT* CRTTMananger::Create_TextureFromResource(const _tchar* pRTT_Tag, ComPtr<ID3D12Resource> pResource)
{
	CRTT* pRTT = CRTT::Create(pRTT_Tag,pResource);

	return pRTT;
}

void CRTTMananger::Free()
{
	for (auto& iter : m_mapRTT)
	{
		for (auto& iter_ : iter.second)
		{
			Safe_Release(iter_);
		}
		iter.second.clear();
	}
	m_mapRTT.clear();
}
