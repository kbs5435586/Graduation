#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"

#include "Debug_Camera.h"

#include "MyRect.h"

unsigned __stdcall ResourceLoadThread(void* pArguments)
{

	CScene_Logo* pLogo = reinterpret_cast<CScene_Logo*>(pArguments);
	EnterCriticalSection(&(pLogo->m_tCritical_Section));





	LeaveCriticalSection(&(pLogo->m_tCritical_Section));
	return 0;
}

CScene_Logo::CScene_Logo()
{
}

HRESULT CScene_Logo::Ready_Scene()
{
	InitializeCriticalSection(&m_tCritical_Section);
	m_hThread_Handle = (HANDLE)_beginthreadex(nullptr, 0, ResourceLoadThread, this, 0, nullptr);

	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();



	if (FAILED(Ready_Prototype_Component(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Light(pManagement)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

_int CScene_Logo::Update_Scene(const _float& fTimeDelta)
{



	return CScene::Update_Scene(fTimeDelta);
}

_int CScene_Logo::LastUpdate_Scene(const _float& fTimeDelta)
{
	//if (GetKeyState(VK_SPACE) & 0x8000)
//{
//	WaitForSingleObject(m_hThread, INFINITE);
//	CloseHandle(m_hThread);
//	DeleteCriticalSection(&m_CritSec);
//	CManagement* pManagement = CManagement::GetInstance();
//	if (nullptr == pManagement)
//		return -1;
//	pManagement->AddRef();
//	CScene_Stage* pNewScene = CScene_Stage::Create(m_pGraphic_Device);
//	if (nullptr == pNewScene)
//		return -1;
//	if (FAILED(pManagement->SetUp_ScenePointer(pNewScene)))
//		return -1;
//	Safe_Release(pNewScene);
//	Safe_Release(pManagement);
//	return 0;
//}

	return CScene::LastUpdate_Scene(fTimeDelta);
}

void CScene_Logo::Render_Scene()
{
}

HRESULT CScene_Logo::Ready_Prototype_GameObject(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MyRect", CMyRect::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CDebug_Camera::Create())))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_Component(CManagement* pManagement)
{
	if (FAILED(Ready_Add_Prototype_Shader(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Buffer(pManagement)))
		return E_FAIL;



	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer(CManagement* pManagement)
{
	if (FAILED(Ready_Layer_Debug_Camera(L"Layer_Debug_Camera", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Rect(L"Layer_MyRect", pManagement)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Logo::Ready_Light(CManagement* pManagement)
{

	return S_OK;
}

HRESULT	CScene_Logo::Ready_Layer_Rect(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_MyRect", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	return S_OK;
}
HRESULT	CScene_Logo::Ready_Layer_Debug_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{
	CDebug_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Debug", (_uint)SCENEID::SCENE_LOGO, pLayerTag, (CGameObject**)&pCameraObject)))
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


HRESULT CScene_Logo::Ready_Add_Prototype_Buffer(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_LOGO, L"Component_Buffer_RectCol",
		CBuffer_RectCol::Create())))
		return E_FAIL;

	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Shader(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_LOGO, L"Component_Shader_Default",
		CShader::Create(L"../ShaderFiles/Shader_Default.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	return S_OK;
}
CScene_Logo* CScene_Logo::Create()
{
	CScene_Logo* pInstance = new CScene_Logo();

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Logo Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScene_Logo::Free()
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return;

	pManagement->AddRef();

	pManagement->Clear_Layers((_uint)SCENEID::SCENE_LOGO);

	Safe_Release(pManagement);
	CScene::Free();
}
