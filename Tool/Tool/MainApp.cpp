#include "stdafx.h"
#include "MainApp.h"
#include "Input.h"
#include "Management.h"
#include "Renderer.h"
#include "Tool_Scene.h"
#include "Camera_Debug.h"
#include "MainFrm.h"
#include "SecondWnd.h"
#include "UnitTool.h"

CMainApp::CMainApp()
	: m_pManagement(CManagement::GetInstance())
{
	m_pManagement->AddRef();
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_Default_Setting(WINMODE_WIN, g_iBackCX, g_iBackCY)))
		return E_FAIL;
	if (FAILED(Ready_Render_State()))
		return E_FAIL;
	if (FAILED(Ready_Sampler_State()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if (FAILED(Ready_Start_Scene(SCENE_LOGO)))
		return E_FAIL;
// 	if (FAILED(Ready_Test_NaviMesh()))
// 		return E_FAIL;

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float & fTimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

	m_fTimeAcc += fTimeDelta;

 	CInput::GetInstance()->SetUp_InputState();
	m_pManagement->Key_Update();

	if(m_pManagement->KeyDown(KEY_F1))
		IsMouseCheck^=true;
	if (m_pManagement->KeyDown(KEY_F2))
		IsEditMode ^= true;
	if (m_pManagement->KeyDown(KEY_U))
		IsWireFrame ^= true;

	return m_pManagement->Update_Management(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	if (nullptr == m_pGraphic_Device)
		return;
	if (nullptr == m_pManagement)
		return;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (nullptr == pMainFrm)
		return;
	CSecondWnd* pSecondWnd = dynamic_cast<CSecondWnd*>(pMainFrm->m_MainSplt.GetPane(0, 1));
	if (nullptr == pSecondWnd)
		return;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	m_pGraphic_Device->BeginScene();
	if (nullptr != m_pRenderer)
		m_pRenderer->Render_RenderGroup();

	m_pManagement->Render_Management();
	m_pGraphic_Device->EndScene();
	m_pGraphic_Device->Present(nullptr, nullptr, 0, nullptr);

	++m_dwRenderCnt;

	if (IsEditMode)
		m_pEdit_Mode = L"EditMode_ON";
	else
		m_pEdit_Mode = L"EditMode_OFF";

	pMainFrm->SetWindowTextW(m_pEdit_Mode);
}

HRESULT CMainApp::Ready_Default_Setting(WINMODE eMode, const _uint & iSizeX, const _uint & iSizeY)
{
	if (FAILED(CDevice::GetInstance()->Ready_Graphic_Device(eMode, g_hWnd, iSizeX, iSizeY, &m_pGraphic_Device)))
		return E_FAIL;
	if (FAILED(CInput::GetInstance()->Ready_Input_Device(g_hInst, AfxGetMainWnd()->m_hWnd)))
 		return E_FAIL;
	if (FAILED(m_pManagement->Ready_Management(SCENE_END)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
 	if (FAILED(m_pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CCamera_Debug::Create(m_pGraphic_Device))))
 		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (FAILED(m_pManagement->Add_Prototype_Component(SCENE_STATIC, L"Component_Transform", CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Prototype_Component(SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;
	m_pRenderer->AddRef();
	if (FAILED(m_pManagement->Add_Prototype_Component(SCENE_STATIC, L"Component_Buffer_RcTex", CBuffer_RcTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Start_Scene(SCENEID eSceneID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CScene*			pScene = nullptr;

	switch (eSceneID)
	{
	case SCENE_LOGO:
		pScene = CTool_Scene::Create(m_pGraphic_Device);
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

HRESULT CMainApp::Ready_Render_State()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	return S_OK;
}

HRESULT CMainApp::Ready_Sampler_State()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CMainApp::Ready_Test_NaviMesh()
{
	HANDLE	hFile = 0;
	_ulong	dwByte = 0;

	hFile = CreateFile(L"../Data/Navi.dat", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_uint			iNumCells = 4;

	WriteFile(hFile, &iNumCells, sizeof(_uint), &dwByte, nullptr);

	_vec3	vPoint[3];

	vPoint[0] = _vec3(0.0f, 0.0f, 5.0f);
	vPoint[1] = _vec3(5.0f, 0.0f, 0.0f);
	vPoint[2] = _vec3(0.0f, 0.0f, 0.0f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);

	vPoint[0] = _vec3(0.0f, 0.0f, 10.0f);
	vPoint[1] = _vec3(5.0f, 0.0f, 5.0f);
	vPoint[2] = _vec3(0.0f, 0.0f, 5.0f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);

	vPoint[0] = _vec3(0.0f, 0.0f, 5.0f);
	vPoint[1] = _vec3(5.0f, 0.0f, 5.0f);
	vPoint[2] = _vec3(5.0f, 0.0f, 0.0f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);

	vPoint[0] = _vec3(5.0f, 0.0f, 5.0f);
	vPoint[1] = _vec3(10.0f, 0.0f, 0.0f);
	vPoint[2] = _vec3(5.0f, 0.0f, 0.0f);
	WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pManagement);
	Safe_Release(m_pGraphic_Device);
	CManagement::Release_Engine();
}
