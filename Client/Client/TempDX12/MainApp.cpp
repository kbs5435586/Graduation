#include "framework.h"
#include "MainApp.h"
#include "Device.h"
#include "Renderer.h"
#include "Management.h"

#include "Scene_Logo.h"
#include "Debug_Camera.h"

CMainApp::CMainApp()
	: m_pManagement(CManagement::GetInstance())
{
	m_pManagement->AddRef();
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_Device()))
		return E_FAIL;
	if (FAILED(Create_FbxManager()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if (FAILED(Ready_Start_Scene(SCENE_LOGO)))
		return E_FAIL;
	if (FAILED(CInput::GetInstance()->Ready_Input_Device(g_hInstance, g_hWnd)))
		return E_FAIL;


	srand(unsigned(time(NULL)));
	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

	m_fTimeAcc += fTimeDelta;
	CInput::GetInstance()->SetUp_InputState();

	return m_pManagement->Update_Management(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	if (nullptr == m_pManagement)
		return;
	m_pDevice->Begin();
	m_pManagement->Render_Management();

	if (nullptr != m_pRenderer)
		m_pRenderer->Render_RenderGroup();

	m_pDevice->End();
	ComputeFrame();
}



HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Prototype_Component(SCENE_STATIC, L"Component_Transform", CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Prototype_Component(SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;
	m_pRenderer->AddRef();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (FAILED(m_pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CDebug_Camera::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}
void CMainApp::ComputeFrame()
{
	++m_dwRenderCnt;
	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS:%d", m_dwRenderCnt);
		m_dwRenderCnt = 0;
		m_fTimeAcc = 0.f;
	}

	SetWindowText(g_hWnd, m_szFPS);
}
HRESULT CMainApp::Create_FbxManager()
{
	if (nullptr != g_FbxManager && g_FbxIOSetting != nullptr)
		return E_FAIL;
	g_FbxManager = FbxManager::Create();
	
	g_FbxIOSetting = FbxIOSettings::Create(g_FbxManager, IOSROOT);

	g_FbxManager->SetIOSettings(g_FbxIOSetting);
	FbxString	strFbxPath = FbxGetApplicationDirectory();
	g_FbxManager->LoadPluginsDirectory(strFbxPath.Buffer());

	return S_OK;
}
HRESULT CMainApp::Ready_Device()
{
	m_pDevice = CDevice::GetInstance();
	NULL_CHECK_VAL(m_pDevice, E_FAIL);
	m_pDevice->AddRef();
	if (FAILED(m_pDevice->OnCreate()))
		return E_FAIL;


	m_pGraphic_Device = m_pDevice->GetDevice();
	NULL_CHECK_VAL(m_pGraphic_Device, E_FAIL);
	m_pGraphic_Device->AddRef();


	if (FAILED(m_pManagement->Ready_Management(SCENE_END)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Start_Scene(SCENEID eSceneID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CScene* pScene = nullptr;

	switch (eSceneID)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pGraphic_Device);
		break;
	case SCENE_STAGE:
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_ScenePointer(pScene)))
		return E_FAIL;

	Safe_Release(pScene);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();
	if (FAILED(pInstance->Ready_MainApp()))
	{
		MessageBox(0, L"CMainApp Created Failed", L"System Error", MB_OK);

		if (nullptr != pInstance)
		{
			delete pInstance;
			pInstance = nullptr;
		}
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pManagement);
	Safe_Release(m_pRenderer);

	m_pManagement->Release_Engine();
}
