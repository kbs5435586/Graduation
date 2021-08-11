#include "framework.h"
#include "LoadManager.h"
#include "Management.h"
#include "Transform.h"
#include "GameObject.h"
#include "Layer.h"


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

	_matrix	mat;
	_float  fAdd_PosY = 0.f;
	mat = Matrix_::Identity();
	TCHAR szName[512] = L"";
	TCHAR szLayerTag[512] = L"";
	TCHAR szComTag[512] = L"";

	DWORD dwByte = 0;
	DWORD dwByte_Size = 0;
	int iLength = 0;
	int iLength_Layer = 0;
	int iLength_Com = 0;

	while (TRUE)
	{
		int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(int), &dwByte_Size, nullptr);
		if (dwByte_Size == 0)
			break;

		for (int i = 0; i < iSize; i++)
		{
			ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR) * iLength, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szLayerTag, sizeof(TCHAR) * iLength_Layer, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Com, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szComTag, sizeof(TCHAR) * iLength_Com, &dwByte, nullptr);


			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Building", (_uint)SCENEID::SCENE_STAGE, L"Layer_Building", nullptr, szComTag)))
			{
				return E_FAIL;

			}

			ReadFile(hFile, &mat, sizeof(_matrix), &dwByte, nullptr);
			CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_BackObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Building")->Get_ComponentPointer(L"Com_Transform");
			pTransform->SetUp_RotationX(XMConvertToRadians(90.f));
			pTransform->Set_Matrix(mat, true);

			ReadFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
			pTransform->Set_Add_PosY(fAdd_PosY);
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoadManager::Load_File_Low(const _tchar* pFilePath, void* pArg)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_matrix	mat;
	_float  fAdd_PosY = 0.f;
	mat = Matrix_::Identity();
	TCHAR szName[512] = L"";
	TCHAR szLayerTag[512] = L"";
	TCHAR szComTag[512] = L"";

	DWORD dwByte = 0;
	DWORD dwByte_Size = 0;
	int iLength = 0;
	int iLength_Layer = 0;
	int iLength_Com = 0;

	while (TRUE)
	{
		int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(int), &dwByte_Size, nullptr);
		if (dwByte_Size == 0)
			break;

		for (int i = 0; i < iSize; i++)
		{
			ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR) * iLength, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szLayerTag, sizeof(TCHAR) * iLength_Layer, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Com, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szComTag, sizeof(TCHAR) * iLength_Com, &dwByte, nullptr);




			CGameObject* pGameObject = nullptr;
			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_LowPoly", (_uint)SCENEID::SCENE_STAGE, L"Layer_Lowpoly", &pGameObject, szComTag)))
			{
				return E_FAIL;
			}
			pGameObject->GetEnviType() = *(ENVITYPE*)pArg;
			ReadFile(hFile, &mat, sizeof(_matrix), &dwByte, nullptr);
			CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_BackObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Lowpoly")->Get_ComponentPointer(L"Com_Transform");
			ReadFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
			if (nullptr == pTransform)
				continue;

			pTransform->SetUp_RotationX(XMConvertToRadians(90.f));
			pTransform->Set_Matrix(mat, true);



			if (pGameObject->GetEnviType() == ENVITYPE::ENVI_FLOWER)
			{
				pTransform->Scaling(4.f, 4.f, 4.f);
			}
			else if (pGameObject->GetEnviType() == ENVITYPE::ENVI_PLANT)
			{

				pTransform->Scaling(4.f, 4.f, 4.f);
			}
			else
			{
				_uint iSize = rand() % 20 + 5;
				_uint iRot = rand() % 20;
				pTransform->Scaling(iSize, iSize, iSize);
			}

			//pTransform->SetUp_RotationY(XMConvertToRadians(180));


			pTransform->Set_Add_PosY(fAdd_PosY);
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoadManager::Load_File_Hatch(const _tchar* pFilePath, void* pArg)
{//GameObject_Hatch
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_matrix	mat;
	_float  fAdd_PosY = 0.f;
	mat = Matrix_::Identity();
	TCHAR szName[512] = L"";
	TCHAR szLayerTag[512] = L"";
	TCHAR szComTag[512] = L"";

	DWORD dwByte = 0;
	DWORD dwByte_Size = 0;
	int iLength = 0;
	int iLength_Layer = 0;
	int iLength_Com = 0;

	while (TRUE)
	{
		int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(int), &dwByte_Size, nullptr);
		if (dwByte_Size == 0)
			break;

		for (int i = 0; i < iSize; i++)
		{
			ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR) * iLength, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szLayerTag, sizeof(TCHAR) * iLength_Layer, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Com, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szComTag, sizeof(TCHAR) * iLength_Com, &dwByte, nullptr);


			_tchar* pComrTag = new _tchar[iLength_Com + 1];
			ZeroMemory(pComrTag, iLength_Com + 1);


			lstrcpy(pComrTag, szComTag);

			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Hatch", (_uint)SCENEID::SCENE_STAGE, L"Layer_Hatch", nullptr, pComrTag)))
			{
				return E_FAIL;

			}

			ReadFile(hFile, &mat, sizeof(_matrix), &dwByte, nullptr);
			CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_BackObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Hatch")->Get_ComponentPointer(L"Com_Transform");
			//pTransform->SetUp_RotationX(XMConvertToRadians(90.f));
			pTransform->Set_Matrix(mat, true);
			//pTransform->Scaling(20.f, 20.f, 20.f);

			ReadFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
			pTransform->Set_Add_PosY(fAdd_PosY);
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoadManager::Load_File_Castle(const _tchar* pFilePath, void* pArg)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_matrix	mat;
	_float  fAdd_PosY = 0.f;
	mat = Matrix_::Identity();
	TCHAR szName[512] = L"";
	TCHAR szLayerTag[512] = L"";
	TCHAR szComTag[512] = L"";

	DWORD dwByte = 0;
	DWORD dwByte_Size = 0;
	int iLength = 0;
	int iLength_Layer = 0;
	int iLength_Com = 0;

	while (TRUE)
	{
		int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(int), &dwByte_Size, nullptr);
		if (dwByte_Size == 0)
			break;

		for (int i = 0; i < iSize; i++)
		{
			ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR) * iLength, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szLayerTag, sizeof(TCHAR) * iLength_Layer, &dwByte, nullptr);

			ReadFile(hFile, &iLength_Com, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szComTag, sizeof(TCHAR) * iLength_Com, &dwByte, nullptr);

			_tchar* pName = new _tchar[iLength + 1];
			ZeroMemory(pName, iLength + 1);

			_tchar* pLayerTag = new _tchar[iLength_Layer + 1];
			ZeroMemory(pLayerTag, iLength_Layer + 1);


			_tchar* pComrTag = new _tchar[iLength_Com + 1];
			ZeroMemory(pComrTag, iLength_Com + 1);

			lstrcpy(pName, szName);
			lstrcpy(pLayerTag, szLayerTag);
			lstrcpy(pComrTag, szComTag);

			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Castle", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, pComrTag)))
			{
				return E_FAIL;

			}

			ReadFile(hFile, &mat, sizeof(_matrix), &dwByte, nullptr);
			CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_BackObject((_uint)SCENEID::SCENE_STAGE, pLayerTag)->Get_ComponentPointer(L"Com_Transform");
			//pTransform->SetUp_RotationX(XMConvertToRadians(90.f));
			pTransform->Set_Matrix(mat, true);
			//pTransform->Scaling(20.f, 20.f, 20.f);

			ReadFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
			pTransform->Set_Add_PosY(fAdd_PosY);
		}
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
