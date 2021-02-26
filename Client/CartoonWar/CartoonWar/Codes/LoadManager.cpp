#include "framework.h"
#include "LoadManager.h"

_IMPLEMENT_SINGLETON(CLoadManager)

CLoadManager::CLoadManager()
{

}

HRESULT CLoadManager::Ready_LoadManager()
{
	return S_OK;
}

HRESULT CLoadManager::Load_File(const _tchar* pFilePath, void* pArg)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	INFO pInfo;

	while (1)
	{
		if (dwByte == 0)
			break;

	}
	CloseHandle(hFile);

	return S_OK;
}

CLoadManager* CLoadManager::Create()
{
	CLoadManager* pInstance = new CLoadManager();
	if (FAILED(pInstance->Ready_LoadManager()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoadManager::Free()
{
}
