#include "framework.h"
#include "MainApp.h"
#include "Management.h"
#include "Renderer.h"
#include "Scene_Logo.h"


CMainApp::CMainApp()
	: m_pManagement(CManagement::GetInstance())
{
	m_pManagement->AddRef();
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_Device()))
		return E_FAIL;
	if (FAILED(m_pManagement->Ready_Management((_uint)SCENEID::SCENE_END)))
		return E_FAIL;
	if (FAILED(Create_FbxManager()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if (FAILED(Ready_Start_Scene(SCENEID::SCENE_LOGO)))
		return E_FAIL;
	if (FAILED(CInput::GetInstance()->Ready_Input_Device(g_hInstance, g_hWnd)))
		return E_FAIL;


	srand(unsigned(time(NULL)));


	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	if (nullptr == m_pManagement)
		return - 1;

	m_fTimeAcc += fTimeDelta;
	CInput::GetInstance()->SetUp_InputState();
	return m_pManagement->Update_Management(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	float pfClearColor[4] = { 0.f, 0.f, 1.f, 1.f };
	CDevice::GetInstance()->Render_Begin(pfClearColor);

	if (nullptr != m_pRenderer)
		m_pRenderer->Render_RenderGroup();

	CDevice::GetInstance()->Render_End();
	Compute_Frame();
}

HRESULT CMainApp::Ready_Device()
{
	if (FAILED(CDevice::GetInstance()->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Start_Scene(SCENEID eID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CScene* pScene = nullptr;

	switch (eID)
	{
	case SCENEID::SCENE_LOGO:
		pScene = CScene_Logo::Create();
		break;
	case SCENEID::SCENE_STAGE:
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_ScenePointer(pScene)))
		return E_FAIL;

	Safe_Release(pScene);

	return S_OK;
}

void CMainApp::Compute_Frame()
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
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform", CTransform::Create())))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create())))
		return E_FAIL;
	m_pRenderer->AddRef();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
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
	Safe_Release(m_pManagement);
	Safe_Release(m_pRenderer);

	m_pManagement->Release_Engine();
}