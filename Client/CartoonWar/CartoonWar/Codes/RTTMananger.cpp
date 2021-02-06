#include "framework.h"
#include "RTTMananger.h"

_IMPLEMENT_SINGLETON(CRTTMananger)

CRTTMananger::CRTTMananger()
{

}

HRESULT CRTTMananger::Ready_RTTMananger(const _tchar* pRTT_Tag, _uint iTextureWidth, _uint iTextureHeight)
{
	auto iter = m_mapRTT.find(pRTT_Tag);
	if (iter != m_mapRTT.end())
		return E_FAIL;

	CRTT* pRTT = CRTT::Create(iTextureWidth, iTextureHeight);
	if (nullptr == pRTT)
		return E_FAIL;
	m_mapRTT.insert({pRTT_Tag, pRTT});

	return S_OK;
}

void CRTTMananger::Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv)
{
	auto iter = m_mapRTT.find(pRTT_Tag);
	if (iter == m_mapRTT.end())
		return;

	iter->second->Set_RenderTarget(pDsv);
}

CRTT* CRTTMananger::Get_RTT(const _tchar* pRTT_Tag)
{
	auto iter = m_mapRTT.find(pRTT_Tag);
	if (iter == m_mapRTT.end())
		return nullptr;

	return iter->second;
}

CRTTMananger* CRTTMananger::Create(const _tchar* pRTT_Tag, _uint iTextureWidth, _uint iTextureHeight)
{
	CRTTMananger* pInstance = new CRTTMananger();
	if (FAILED(pInstance->Ready_RTTMananger(pRTT_Tag, iTextureWidth, iTextureHeight)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRTTMananger::Free()
{
	for (auto& iter : m_mapRTT)
	{
		Safe_Release(iter.second);
	}
	m_mapRTT.clear();
}
