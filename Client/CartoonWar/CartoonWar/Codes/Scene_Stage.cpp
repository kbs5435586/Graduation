#include "framework.h"
#include "Scene_Stage.h"
#include "Management.h"

#include "Cube.h"
#include "SkyBox.h"
#include "Debug_Camera.h"
#include "Terrain.h"

CScene_Stage::CScene_Stage()
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(Ready_Prototype_GameObject(pManagement)))
		return E_FAIL;

	if (FAILED(Ready_Light(pManagement)))
		return E_FAIL;

	if (FAILED(Ready_Layer(pManagement)))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}

_int CScene_Stage::Update_Scene(const _float& fTimeDelta)
{
	return CScene::Update_Scene(fTimeDelta);
}

_int CScene_Stage::LastUpdate_Scene(const _float& fTimeDelta)
{
	return CScene::LastUpdate_Scene(fTimeDelta);
}

void CScene_Stage::Render_Scene()
{
}

HRESULT CScene_Stage::Ready_Prototype_GameObject(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cube", CCube::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CDebug_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain", CTerrain::Create())))
		return E_FAIL;
	return S_OK;
}


HRESULT CScene_Stage::Ready_Layer(CManagement* pManagement)
{
	if (FAILED(Ready_Layer_BasicShape(L"Layer_BasicShape", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Debug_Camera(L"Layer_Camera_Debug", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain", pManagement)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Light(CManagement* pManagement)
{
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BasicShape(const _tchar* pLayerTag, CManagement* pManagement)
{

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Cube", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Debug_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{

	CDebug_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Debug", (_uint)SCENEID::SCENE_STAGE, pLayerTag,
		(CGameObject**)&pCameraObject)))
		return E_FAIL;

	CAMERADESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERADESC));
	tCameraDesc.vEye = _vec3(0.f, 0.f, -5.f);
	tCameraDesc.vAt = _vec3(0.f, 0.f, 1.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);
	PROJDESC		tProjDesc;
	ZeroMemory(&tProjDesc, sizeof(tProjDesc));
	tProjDesc.fFovY = XMConvertToRadians(60.f);
	tProjDesc.fAspect = _float(WINCX) / WINCY;
	tProjDesc.fNear = g_Near;
	tProjDesc.fFar = g_Far;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_SkyBox(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_SkyBox", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Terrain(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}



CScene_Stage* CScene_Stage::Create()
{
	CScene_Stage* pInstance = new CScene_Stage();

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Stage Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScene_Stage::Free()
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return;

	pManagement->AddRef();

	pManagement->Clear_Layers((_uint)SCENEID::SCENE_STAGE);

	Safe_Release(pManagement);
	CScene::Free();
}