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
	CUAV* pUAV = CUAV::Create(1000.f, 1000.f, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
							, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	m_mapUAV.insert({ L"UAV_Default", pUAV });

	pUAV = CUAV::Create(1000.f, 1000.f, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	m_mapUAV.insert({ L"UAV_Default_1", pUAV });

	pUAV = CUAV::Create(1000.f, 1000.f, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	m_mapUAV.insert({ L"UAV_Default_2", pUAV });

	pUAV = CUAV::Create(1000.f, 1000.f, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	m_mapUAV.insert({ L"UAV_Default_3", pUAV });

	pUAV = CUAV::Create(1000.f, 1000.f, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	m_mapUAV.insert({ L"UAV_Default_4", pUAV });

	return S_OK;
}

CUAV* CUAVManager::GetUAV(const _tchar* pTag)
{
	auto iter = find_if(m_mapUAV.begin(), m_mapUAV.end(), CFinder_Tag(pTag));
	if (iter == m_mapUAV.end())
		return nullptr;

	return iter->second;
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
