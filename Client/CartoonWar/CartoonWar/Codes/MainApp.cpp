#include "framework.h"
#include "MainApp.h"
#include "Management.h"
#include "Renderer.h"
#include "Scene_Logo.h"
#include "Scene_Stage.h"


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
	if (FAILED(Ready_Sound()))
		return E_FAIL;


	if (FAILED(m_pManagement->Ready_RTT_Manager()))
		return E_FAIL;
	if (FAILED(m_pManagement->Ready_UAV_Manager()))
		return E_FAIL;
	if (FAILED(m_pManagement->Ready_CollsionManager()))
		return E_FAIL;
	if (FAILED(m_pManagement->Ready_FontMgr("../Bin/Resource/Texture/Font/fontdata.txt")))
		return E_FAIL;




	if (FAILED(CInput::GetInstance()->Ready_Input_Device(g_hInstance, g_hWnd)))
		return E_FAIL;

	
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(MAINPASS), 512, CONST_REGISTER::b0)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(MATERIAL), 512, CONST_REGISTER::b1)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(LIGHTINFO), 1, CONST_REGISTER::b2, true)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(REFLECT), 512, CONST_REGISTER::b3)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(TEXINFO), 512, CONST_REGISTER::b4)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(DISTORTION), 512, CONST_REGISTER::b5)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(FOG), 512, CONST_REGISTER::b6)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(TIME), 512, CONST_REGISTER::b7)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(REP), 512, CONST_REGISTER::b8)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(GLOBAL), 1, CONST_REGISTER::b9, true)))
		return E_FAIL;
	if (FAILED(m_pManagement->Create_Constant_Buffer(sizeof(BRUSHINFO), 512, CONST_REGISTER::b10)))
		return E_FAIL;

	if (FAILED(Ready_Start_Scene(SCENEID::SCENE_LOGO)))
		return E_FAIL;

	srand(unsigned(time(NULL)));

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	if (nullptr == m_pManagement)
		return - 1;
	if (g_IsStageStart)
	{
		m_fGoldTime += fTimeDelta;
		if (m_fGoldTime >= 10.f)
		{
			g_iGold++;
			m_fGoldTime = 0.f;
		}

	}

	if (g_iGold >= 9)
		g_iGold = 9;
	if (g_iTotalTime >= 300.f)
	{
		g_IsEnd = true;
		if (g_iRedNum >= g_iBlueNum)
		{
			// Red Win
			g_IsWin = true;
		}
		else
		{
			// Blue Win
			g_IsWin = false;
		}
	}

	//if (GetAsyncKeyState('O'))
	//{
	//	g_iTotalTime = 300.f;
	//}
	//if (GetAsyncKeyState('P'))
	//{
	//	if (g_DefferedRender == 0)
	//	{
	//		g_DefferedRender = 1;
	//	}
	//	else
	//	{
	//		g_DefferedRender = 0;
	//	}
	//}
	
	m_fTimeAcc += fTimeDelta;
	m_fTimeDelta = fTimeDelta;
	CInput::GetInstance()->SetUp_InputState();
	m_pFrustum->Transform_ToWorld();
	m_pManagement->Update_Sound();

	SetTime(fTimeDelta);
	return m_pManagement->Update_Management(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	CDevice::GetInstance()->Render_Begin();
	if (FAILED(SetUp_OnShader(m_fTimeDelta)))
		return ;
	m_pManagement->SetUp_OnShader_Light();
	if (nullptr != m_pRenderer)
		m_pRenderer->Render_RenderGroup();


	CDevice::GetInstance()->Render_End();

 	for (auto& iter : m_pManagement->GetConstantBuffer())
		iter->ResetCount();

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
		pScene = CScene_Stage::Create();
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_ScenePointer(pScene)))
		return E_FAIL;

	Safe_Release(pScene);

	return S_OK;
}

HRESULT CMainApp::Ready_Sound()
{
	if (FAILED(m_pManagement->Ready_Channel()))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_BG, LOGO, "../Bin/Resource/Sounds/Tittle.mp3", 0.1f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_BG, BG, "../Bin/Resource/Sounds/BattleHighLand.mp3", 0.1f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, HIT, "../Bin/Resource/Sounds/Hit.wav", 0.5f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, DIE, "../Bin/Resource/Sounds/Orc_Death.wav", 0.5f)))
		return E_FAIL;	
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, HITTED, "../Bin/Resource/Sounds/Orc_Hitted.wav", 0.5f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, ATTACK, "../Bin/Resource/Sounds/Sword_Whoosh_Attack_1.wav", 0.3f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, WALK, "../Bin/Resource/Sounds/WR1.wav", 0.5f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, RUN, "../Bin/Resource/Sounds/Walk_Run.wav", 0.5f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, HORSE_RUN, "../Bin/Resource/Sounds/RunHorse.mp3", 0.5f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, HORSE_WALK, "../Bin/Resource/Sounds/WalkHorse.mp3", 0.5f)))
		return E_FAIL;	
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, IDLE, "../Bin/Resource/Sounds/Idle_Orc_Breath.wav", 0.05f)))
		return E_FAIL;
	if (FAILED(m_pManagement->Add_Sound(SOUND_OBJECT, SHOOT, "../Bin/Resource/Sounds/Arrow.wav", 0.5f)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::SetUp_OnShader(const _float& fTimeDelta)
{
	m_tGlobal.fTimeDelta = fTimeDelta;
	m_tGlobal.fAccTime += fTimeDelta;
	m_tGlobal.iWincx = WINCX;
	m_tGlobal.iWincy = WINCY;

	g_MaxTime -= fTimeDelta;

	_uint iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b9)->SetData(&m_tGlobal);
	CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b9)->GetCBV().Get(), iOffset, CONST_REGISTER::b9);

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

void CMainApp::SetTime(const _float& fTimeDelta)
{
	if (m_fDatePer >= 1.f)
	{
		m_fDatePer = 0.f;
		m_fDateTime = 0.f;
		return;
	}

	m_fDateTime = g_iTotalTime;
	m_fDatePer = m_fDateTime / 300.f;

	_float fTemp = Lerp(1.f, -1.2f, m_fDatePer);
	g_iDiffusePer = fTemp;
}

HRESULT CMainApp::Create_FbxManager()
{
	FbxIOSettings* pIOSetting = nullptr;
	if (nullptr != g_FbxManager && pIOSetting != nullptr)
		return E_FAIL;

	g_FbxManager = FbxManager::Create();
	pIOSetting = FbxIOSettings::Create(g_FbxManager, IOSROOT);

	g_FbxManager->SetIOSettings(pIOSetting);
	FbxString	strFbxPath = FbxGetApplicationDirectory();
	g_FbxManager->LoadPluginsDirectory(strFbxPath.Buffer());

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

	if (FAILED(m_pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum", m_pFrustum= CFrustum::Create())))
		return E_FAIL;
	m_pFrustum->AddRef();

	//if (FAILED(m_pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex_RTT",CBuffer_RectTex::Create())))
	//	return E_FAIL;
	//if (FAILED(m_pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_RTT",CShader::Create(L"../ShaderFiles/Shader_RTT.hlsl", "VS_Main", "PS_Main"))))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	return S_OK;
}

_float CMainApp::Lerp(_float a, _float b, _float val)
{
	_float fTemp = a * (1.f - val);
	_float fTemp_= b * val;
	return fTemp + fTemp_;
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
	Safe_Release(m_pFrustum);

	m_pManagement->Release_Engine();
}
