#include "framework.h"
#include "UAVManager.h"
#include "UAV.h"

_IMPLEMENT_SINGLETON(CUAVManager)
CUAVManager::CUAVManager()
{
}

HRESULT CUAVManager::Ready_UAVManager()
{
	// Create UAV Texture
	CUAV* pUAV = CUAV::Create(1024,1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
							, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);


	m_mapUAV.insert({ L"UAV_Default", pUAV });
	return S_OK;
}

CUAV* CUAVManager::GetUAV(const _tchar* pTag)
{
	auto iter_find = m_mapUAV.find(pTag);
	if (iter_find == m_mapUAV.end())
		return nullptr;

	return iter_find->second;
}

CUAVManager* CUAVManager::Create()
{
	CUAVManager* pInstance = new CUAVManager();

	if (FAILED(pInstance->Ready_UAVManager()))
		Safe_Release(pInstance);

	return pInstance;
}

void CUAVManager::Free()
{
	for (auto& iter : m_mapUAV)
	{
		Safe_Release(iter.second);
	}
}
