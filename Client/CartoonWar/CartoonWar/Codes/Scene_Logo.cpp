#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"
// GameObject
#include "Logo.h"
// Defferd UI
#include "UI_Main.h"
#include "UI_Diffuse.h"
#include "UI_Normal.h"
#include "UI_Position.h"
#include "UI_Shade.h"
#include "UI_PointLight.h"


// New Scene
#include "Scene_Stage.h"

#include "Buffer_Terrain.h"
#include "Buffer_Cylinder.h"


unsigned __stdcall ResourceLoadThread(void* pArguments)
{
	CScene_Logo* pLogo = reinterpret_cast<CScene_Logo*>(pArguments);
	EnterCriticalSection(&(pLogo->m_tCritical_Section_Mesh));

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
	{
		LeaveCriticalSection(&(pLogo->m_tCritical_Section_Mesh));
		return 0;
	}

	pManagement->AddRef();

	pLogo->Ready_Add_Prototype_Mesh(pManagement);

	Safe_Release(pManagement);

	LeaveCriticalSection(&(pLogo->m_tCritical_Section_Mesh));
	return 0;
}


unsigned __stdcall ShaderCompileThread(void* pArguments)
{
	CScene_Logo* pLogo = reinterpret_cast<CScene_Logo*>(pArguments);
	EnterCriticalSection(&(pLogo->m_tCritical_Section_Shader));

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
	{
		LeaveCriticalSection(&(pLogo->m_tCritical_Section_Shader));
		return 0;
	}

	pManagement->AddRef();

	pLogo->Ready_Add_Prototype_Shader(pManagement);

	Safe_Release(pManagement);

	LeaveCriticalSection(&(pLogo->m_tCritical_Section_Shader));
	return 0;
}




CScene_Logo::CScene_Logo()
{
	m_pInput = CInput::GetInstance();
	m_pInput->AddRef();
}

HRESULT CScene_Logo::Ready_Scene()
{
	m_eSceneID = SCENEID::SCENE_LOGO;

	InitializeCriticalSection(&m_tCritical_Section_Mesh);
	m_hThread_Handle_Mesh = (HANDLE)_beginthreadex(nullptr, 0, ResourceLoadThread, this, 0, nullptr);

	InitializeCriticalSection(&m_tCritical_Section_Shader);
	m_hThread_Handle_Shader = (HANDLE)_beginthreadex(nullptr, 0, ShaderCompileThread, this, 0, nullptr);

	WaitForSingleObject(m_hThread_Handle_Mesh, INFINITE);
	CloseHandle(m_hThread_Handle_Mesh);
	DeleteCriticalSection(&m_tCritical_Section_Mesh);

	WaitForSingleObject(m_hThread_Handle_Shader, INFINITE);
	CloseHandle(m_hThread_Handle_Shader);
	DeleteCriticalSection(&m_tCritical_Section_Shader);

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

	float drawX = (float)((((_float)WINCX / 2) * -1) + 40);
	float drawY = (float)(((_float)WINCY / 2) - 10);
	
	if (FAILED(pManagement->Create_Font_Buffer(L"IP", m_strIP.c_str(), 0, 0)))
		return E_FAIL;
	drawX = (float)((((_float)WINCX / 2) * -1) + 30);
	drawY = (float)(((_float)WINCY / 2) - 30);
	if (FAILED(pManagement->Create_Font_Buffer(L"ID", m_strID.c_str(), 100, 100)))
		return E_FAIL;

	pManagement->Play_Sound(CHANNEL_BG, SOUND_BG, LOGO, 0.f, FMOD_LOOP_NORMAL);
	Safe_Release(pManagement);
	return S_OK;

}

_int CScene_Logo::Update_Scene(const _float& fTimeDelta)
{
	Input_ID_IP(fTimeDelta);
	return CScene::Update_Scene(fTimeDelta);
}

_int CScene_Logo::LastUpdate_Scene(const _float& fTimeDelta)
{
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		g_strIP = m_strIP;
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return -1;
		pManagement->AddRef();

		CScene_Stage* pNewScene = CScene_Stage::Create();
		if (nullptr == pNewScene)
			return -1;
		if (FAILED(pManagement->SetUp_ScenePointer(pNewScene)))
			return -1;
		Safe_Release(pNewScene);


		Safe_Release(pManagement);
		return 0;
	}
	return CScene::LastUpdate_Scene(fTimeDelta);
}

void CScene_Logo::Render_Scene()
{

}

HRESULT CScene_Logo::Ready_Prototype_GameObject(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Logo", CLogo::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Main", CUI_Main::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Diffuse", CUI_Diffuse::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Normal", CUI_Normal::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Position", CUI_Position::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Shade", CUI_Shade::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_PointLight", CUI_PointLight::Create())))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_Component(CManagement* pManagement)
{

	if (FAILED(Ready_Add_Prototype_Buffer(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Texture(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Function(pManagement)))
		return E_FAIL;



	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer(CManagement* pManagement)
{
	if (FAILED(Ready_Layer_Logo(L"Layer_Logo", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(L"Layer_UI", pManagement)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Light(CManagement* pManagement)
{
	return S_OK;
}


HRESULT CScene_Logo::Ready_Layer_Logo(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Logo", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Diffuse", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

void CScene_Logo::Input_ID_IP(const _float& fTimeDelta)
{
	if (m_pInput->Get_DIKeyState(DIK_RETURN) & 0x80)
		m_IsIP = false;

	m_fInputTime += fTimeDelta;
	if (m_fInputTime >= 0.1f)
	{
		if (m_pInput->Get_DIKeyState(DIK_1) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "1";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "1";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_2) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "2";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "2";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_3) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "3";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "3";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_4) & 0x8000)
		{

			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "4";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "4";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_5) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "5";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "5";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_6) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "6";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "6";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_7) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "7";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "7";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_8) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "8";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "8";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_9) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "9";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "9";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_0) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += "0";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += "0";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_RCONTROL) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP += ".";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}
			else
			{
				CManagement::GetInstance()->Delete_Font(L"ID");
				m_strID += ".";
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
					return;
			}
		}
		else if (m_pInput->Get_DIKeyState(DIK_BACKSPACE) & 0x8000)
		{
			if (m_IsIP)
			{
				CManagement::GetInstance()->Delete_Font(L"IP");
				m_strIP.pop_back();
				if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"IP", m_strIP.c_str(), -60, -70)))
					return;
			}	
		}
		else if (m_pInput->Get_DIKeyState(DIK_Q) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "Q";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_W) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "W";
			float drawX = (float)((((_float)WINCX / 2) * -1) + 30);
			float drawY = (float)(((_float)WINCY / 2) - 30);
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_E) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "E";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_R) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "R";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_T) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "T";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_Y) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "Y";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_U) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "U";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_I) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "I";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_O) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "O";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_P) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "P";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}

		else if (m_pInput->Get_DIKeyState(DIK_A) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "A";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_S) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "S";

			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_D) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "D";

			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_F) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "F";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_G) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "G";

			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_H) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "H";

			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_J) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "J";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_K) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "K";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_L) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "L";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}


		else if (m_pInput->Get_DIKeyState(DIK_Z) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "Z";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_X) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "X";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_C) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "C";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_V) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "V";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_B) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "B";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_N) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "N";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}
		else if (m_pInput->Get_DIKeyState(DIK_N) & 0x8000)
		{
			if (m_IsIP)
				return;
			CManagement::GetInstance()->Delete_Font(L"ID");
			m_strID += "N";
			if (FAILED(CManagement::GetInstance()->Create_Font_Buffer(L"ID", m_strID.c_str(), -60, -110)))
				return;
		}

		m_fInputTime = 0.f;
	}
	

}


HRESULT CScene_Logo::Ready_Add_Prototype_Buffer(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectCol",
		CBuffer_RectCol::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex",
		CBuffer_RectTex::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_CubeCol",
		CBuffer_CubeCol::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_CubeTex",
		CBuffer_CubeTex::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Cylinder",
		CBuffer_Cylinder::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain",
		CBuffer_Terrain::Create(100, 100))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height",
		CBuffer_Terrain_Height::Create(L"../Bin/Resource/Texture/Height/TerrainTest.bmp", 2.f))))
		return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height",
	//	CBuffer_Terrain_Height::Create(L"../Bin/Resource/Texture/Height/tttt.bmp", 1.f))))
	//	return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Sphere",
		CBuffer_Sphere::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Point",
		CBuffer_Point::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTexNor",
		CBuffer_RectTexNor::Create())))
		return E_FAIL;


	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Function(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB",
		CCollider::Create(COLLIDER_TYPE::COLLIDER_AABB))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB",
		CCollider::Create(COLLIDER_TYPE::COLLIDER_OBB))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_ATTACK",
		CCollider::Create(COLLIDER_TYPE::COLLIDER_OBB))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_SPHERE",
		CCollider::Create(COLLIDER_TYPE::COLLIDER_SPHERE))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer",
		CObserver::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Particle",
		CParticle::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation",
		CAnimator::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Picking",
		CPicking::Create(g_hWnd))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh",
		CNavigation::Create(L"../Data/Navi/NaviTest2.dat"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Add_Prototype_Mesh(CManagement* pManagement)
{
	{
		//m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Bear");
		//NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR], E_FAIL);
		//if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_BEAR", m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR])))
		//	return E_FAIL;
		//
		//m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Boar");
		//NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR], E_FAIL);
		//if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_BOAR", m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR])))
		//	return E_FAIL;
		//
		//m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Deer");
		//NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER], E_FAIL);
		//if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_DEER", m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER])))
		//	return E_FAIL;
		//
		//m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Wolf");
		//NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF], E_FAIL);
		//if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_WOLF", m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF])))
		//	return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Bear",
			CMesh::Create_Load(L"../Data/MeshData/Bear.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Boar",
			CMesh::Create_Load(L"../Data/MeshData/Boar.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Deer",
			CMesh::Create_Load(L"../Data/MeshData/Deer.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Wolf",
			CMesh::Create_Load(L"../Data/MeshData/Wolf.dat"))))
			return E_FAIL;
	}
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Archer",
			CMesh::Create_Load(L"../Data/Human/Archer.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Carvalry",
			CMesh::Create_Load(L"../Data/Human/Heavy_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Infantry",
			CMesh::Create_Load(L"../Data/Human/Heavy_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Cavalry",
			CMesh::Create_Load(L"../Data/Human/Light_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Infantry",
			CMesh::Create_Load(L"../Data/Human/Light_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Mage",
			CMesh::Create_Load(L"../Data/Human/Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Mounted_Mage",
			CMesh::Create_Load(L"../Data/Human/Mounted_Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Priest",
			CMesh::Create_Load(L"../Data/Human/Priest.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_SpearMan",
			CMesh::Create_Load(L"../Data/Human/SpaerMan.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Worker",
			CMesh::Create_Load(L"../Data/Human/Worker.dat"))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Archer",
			CMesh::Create_Load(L"../Data/Undead/Archer.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Carvalry",
			CMesh::Create_Load(L"../Data/Undead/Heavy_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Infantry",
			CMesh::Create_Load(L"../Data/Undead/Heavy_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Cavalry",
			CMesh::Create_Load(L"../Data/Undead/Light_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Infantry",
			CMesh::Create_Load(L"../Data/Undead/Light_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mage",
			CMesh::Create_Load(L"../Data/Undead/Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mounted_Mage",
			CMesh::Create_Load(L"../Data/Undead/Mounted_Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_SpearMan",
			CMesh::Create_Load(L"../Data/Undead/SpearMan.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Worker",
			CMesh::Create_Load(L"../Data/Undead/Worker.dat"))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Arrow",
			CMesh::Create_Load(L"../Data/Arrow.dat"))))
			return E_FAIL;
	}


	{
		/*	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_barracks_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_single_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_barracks_single_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_big_storage_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_big_storage_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_bighouse_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_02_low",
				CMesh::Create_Load(L"../Data/MeshData/build_bighouse_02_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_blacksmith_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_blacksmith_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_boat_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_boat_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_gate_01",
				CMesh::Create_Load(L"../Data/MeshData/build_gate_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_small_house_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_straw_roof_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_small_house_straw_roof_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_tall_roof_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_small_house_tall_roof_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_storage_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_storage_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_tower_01",
				CMesh::Create_Load(L"../Data/MeshData/build_tower_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_corner_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_wall_corner_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_panel_01_low",
				CMesh::Create_Load(L"../Data/MeshData/build_wall_panel_01_low.dat"))))
				return E_FAIL;


			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01",
				CMesh::Create_Load(L"../Data/MeshData/MountainRocks01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_A",
				CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_A.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_B",
				CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_B.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_C",
				CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_C.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_D",
				CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_D.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_C",
				CMesh::Create_Load(L"../Data/MeshData/Rock01_C.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_D",
				CMesh::Create_Load(L"../Data/MeshData/Rock01_D.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02",
				CMesh::Create_Load(L"../Data/MeshData/Rock02.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02_A",
				CMesh::Create_Load(L"../Data/MeshData/Rock02_A.dat"))))
				return E_FAIL;


			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02_B",
				CMesh::Create_Load(L"../Data/MeshData/Rock02_B.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock03",
				CMesh::Create_Load(L"../Data/MeshData/Rock03.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Bush01",
				CMesh::Create_Load(L"../Data/MeshData/rpb_Bush01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Bush02",
				CMesh::Create_Load(L"../Data/MeshData/rpb_Bush02.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Cloud1",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Cloud1.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Cloud2",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Cloud2.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Flower1",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Flower1.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Flower2",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Flower2.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Flower3",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Flower3.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Plant1",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Plant1.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Plant2",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Plant2.dat"))))
				return E_FAIL;


			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Rock1",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Rock1.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Rock2",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Rock2.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Rock3",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Rock3.dat"))))
				return E_FAIL;

			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_rpg_Rock4",
				CMesh::Create_Load(L"../Data/MeshData/rpg_Rock4.dat"))))
				return E_FAIL;
				*/
	}

	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Flag_Blue",
			CMesh::Create_Load(L"../Data/MeshData/Flag_Blue.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Flag_Red",
			CMesh::Create_Load(L"../Data/MeshData/Flag_Red.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Brazier01",
			CMesh::Create_Load(L"../Data/MeshData/Brazier01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Bonfire",
			CMesh::Create_Load(L"../Data/MeshData/Brazier02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Castle",
			CMesh::Create_Load(L"../Data/MeshData/Castle.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Deffend",
			CMesh::Create_Load(L"../Data/MeshData/Deffend.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_barracks_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_single_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_barracks_single_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_big_storage_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_big_storage_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_bighouse_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_02_low",
			CMesh::Create_Load(L"../Data/MeshData/build_bighouse_02_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_blacksmith_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_blacksmith_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_boat_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_boat_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_gate_01",
			CMesh::Create_Load(L"../Data/MeshData/build_gate_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_small_house_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_straw_roof_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_small_house_straw_roof_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_tall_roof_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_small_house_tall_roof_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_storage_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_storage_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_tower_01",
			CMesh::Create_Load(L"../Data/MeshData/build_tower_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_corner_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_wall_corner_01_low.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_panel_01_low",
			CMesh::Create_Load(L"../Data/MeshData/build_wall_panel_01_low.dat"))))
			return E_FAIL;




		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_A",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_A.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_B",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_B.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_C",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_C.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_D",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks01_D.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02_A",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks02_A.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02_B",
			CMesh::Create_Load(L"../Data/MeshData/MountainRocks02_B.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks03",
			CMesh::Create_Load(L"../Data/MeshData/MountainsRocks03.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_tree_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_tree_02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_pine_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_tree_pine_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_flower_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_flower_02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_03",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_flower_03.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_plant_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_plant_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_plant_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_plant_02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_rock_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_rock_02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_03",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_rock_03.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_small_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_rock_small_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_small_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_rock_small_02.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_bush_01",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_bush_01.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_bush_02",
			CMesh::Create_Load(L"../Data/MeshData/rpgpp_lt_bush_02.dat"))))
			return E_FAIL;
	}

	{
		/*if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Archer",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Archer.fbx", L"../Data/Human/Archer.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Carvalry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Heavy_Cavalry.fbx", L"../Data/Human/Heavy_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Infantry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Heavy_Infantry.fbx", L"../Data/Human/Heavy_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Cavalry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Light_Cavalry.fbx", L"../Data/Human/Light_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Infantry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Light_Infantry.fbx", L"../Data/Human/Light_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_SpearMan",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Spearman.fbx", L"../Data/Human/SpaerMan.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Mage",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Mage.fbx", L"../Data/Human/Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Mounted_Mage",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Mounted_Mage.fbx", L"../Data/Human/Mounted_Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Priest",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Priest.fbx", L"../Data/Human/Priest.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Worker",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Human/Human_Worker.fbx", L"../Data/Human/Worker.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Cavalry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Heavy_Cavalry.fbx", L"../Data/Undead/Heavy_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Infantry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Heavy_Infantry.fbx", L"../Data/Undead/Heavy_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Cavalry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Light_Cavalry.fbx", L"../Data/Undead/Light_Cavalry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Infantry",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Light_Infantry.fbx", L"../Data/Undead/Light_Infantry.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mage",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Mage.fbx", L"../Data/Undead/Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mounted_Mage",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Mounted_Mage.fbx", L"../Data/Undead/Mounted_Mage.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Spearman",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Spearman.fbx", L"../Data/Undead/SpearMan.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Worker",
			CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Worker.fbx", L"../Data/Undead/Worker.dat"))))
			return E_FAIL;*/
	}
	{
		/*	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_barracks_01_low.fbx", L"../Data/MeshData/build_barracks_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_single_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_barracks_single_01_low.fbx", L"../Data/MeshData/build_barracks_single_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_big_storage_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_big_storage_01_low.fbx", L"../Data/MeshData/build_big_storage_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_bighouse_01_low.fbx", L"../Data/MeshData/build_bighouse_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_bighouse_02_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_bighouse_02_low.fbx", L"../Data/MeshData/build_bighouse_02_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_blacksmith_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_blacksmith_01_low.fbx", L"../Data/MeshData/build_blacksmith_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_boat_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_boat_01_low.fbx", L"../Data/MeshData/build_boat_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_gate_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_gate_01.fbx", L"../Data/MeshData/build_gate_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_small_house_01_low.fbx", L"../Data/MeshData/build_small_house_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_straw_roof_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_small_house_straw_roof_01_low.fbx", L"../Data/MeshData/build_small_house_straw_roof_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_small_house_tall_roof_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_small_house_tall_roof_01_low.fbx", L"../Data/MeshData/build_small_house_tall_roof_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_storage_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_storage_01_low.fbx", L"../Data/MeshData/build_storage_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_tower_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_tower_01.fbx", L"../Data/MeshData/build_tower_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_corner_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_wall_corner_01_low.fbx", L"../Data/MeshData/build_wall_corner_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_wall_panel_01_low",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Buildings/build_wall_panel_01_low.fbx", L"../Data/MeshData/build_wall_panel_01_low.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01.fbx", L"../Data/MeshData/MountainRocks01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_A",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01_A.fbx", L"../Data/MeshData/MountainRocks01_A.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_B",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01_B.fbx", L"../Data/MeshData/MountainRocks01_B.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_C",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01_C.fbx", L"../Data/MeshData/MountainRocks01_C.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks01_D",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01_D.fbx", L"../Data/MeshData/MountainRocks01_D.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks02.fbx", L"../Data/MeshData/MountainRocks02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02_A",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks02_A.fbx", L"../Data/MeshData/MountainRocks02_A.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks02_B",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks02_B.fbx", L"../Data/MeshData/MountainRocks02_B.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_MountainRocks03",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainsRocks03.fbx", L"../Data/MeshData/MountainsRocks03.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_tree_01.fbx", L"../Data/MeshData/rpgpp_lt_bush_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_tree_02.fbx", L"../Data/MeshData/rpgpp_lt_bush_02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_tree_pine_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_tree_pine_01.fbx", L"../Data/MeshData/rpgpp_lt_tree_pine_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_flower_01.fbx", L"../Data/MeshData/rpgpp_lt_flower_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_flower_02.fbx", L"../Data/MeshData/rpgpp_lt_flower_02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_flower_03",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_flower_03.fbx", L"../Data/MeshData/rpgpp_lt_flower_03.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_plant_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_plant_01.fbx", L"../Data/MeshData/rpgpp_lt_plant_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_plant_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_plant_02.fbx", L"../Data/MeshData/rpgpp_lt_plant_02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_rock_01.fbx", L"../Data/MeshData/rpgpp_lt_rock_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_rock_02.fbx", L"../Data/MeshData/rpgpp_lt_rock_02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_03",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_rock_03.fbx", L"../Data/MeshData/rpgpp_lt_rock_03.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_small_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_rock_small_01.fbx", L"../Data/MeshData/rpgpp_lt_rock_small_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_rock_small_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_rock_small_02.fbx", L"../Data/MeshData/rpgpp_lt_rock_small_02.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_bush_01",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_bush_01.fbx", L"../Data/MeshData/rpgpp_lt_bush_01.dat"))))
				return E_FAIL;
			if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_rpgpp_lt_bush_02",
				CMesh::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_bush_02.fbx", L"../Data/MeshData/rpgpp_lt_bush_02.dat"))))
				return E_FAIL;*/
	}

	{
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Deffend",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Static/Mantlet0.fbx", L"../Data/MeshData/Deffend.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Brazier01",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Static/Brazier/Brazier01.fbx", L"../Data/MeshData/Brazier01.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Brazier02",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Static/Brazier/Brazier02.fbx", L"../Data/MeshData/Brazier02.dat"))))
		//	return E_FAIL;
	}

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animal_Deer",
		CMesh::Create_Load(L"../Data/Animal/Deer.dat"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animal_Wolf",
		CMesh::Create_Load(L"../Data/Animal/Wolf.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Logo::Ready_Add_Prototype_Texture(CManagement* pManagement)
{
	//DDS 
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_SkyBox",
			CTexture::Create(L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", 4, TEXTURE_TYPE::TEXTURE_TYPE_DDS, true))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_FireAlpha",
			CTexture::Create(L"../Bin/Resource/Texture/Fire/Fire_alpha%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_FireWallAlpha",
			CTexture::Create(L"../Bin/Resource/Texture/Fire/FireWall_alpha%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Firefire",
			CTexture::Create(L"../Bin/Resource/Texture/Fire/Fire_fire%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Firenoise",
			CTexture::Create(L"../Bin/Resource/Texture/Fire/Fire_noise%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch",
			CTexture::Create(L"../Bin/Resource/Texture/Hatching/Hatch%d.dds", 6, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_WaterNor",
			CTexture::Create(L"../Bin/Resource/Texture/Water/WaterNormal%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
			return E_FAIL;

	}

	//PNGJPG
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Logo",
			CTexture::Create(L"../Bin/Resource/Texture/Logo/Logo%d.jpg", 2, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Noise",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/noise%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_CartoonSmoke",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/CartoonSmoke%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Particle_Hit",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/Particle%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		//
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ClassUI",
			CTexture::Create(L"../Bin/Resource/Texture/UI/panel_blue.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ButtonUI",
			CTexture::Create(L"../Bin/Resource/Texture/UI/buttonSquare_blue.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ButtonUI_Pressed",
			CTexture::Create(L"../Bin/Resource/Texture/UI/buttonSquare_blue_pressed.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_RoundButtonUI",
			CTexture::Create(L"../Bin/Resource/Texture/UI/iconCircle_blue.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_RoundButtonUI_Pressed",
			CTexture::Create(L"../Bin/Resource/Texture/UI/iconCross_blue.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_HPBar",
			CTexture::Create(L"../Bin/Resource/Texture/HPBar/HP%d.png", 3, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Font",
			CTexture::Create(L"../Bin/Resource/Texture/Font/Font%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_BrushTemp",
			CTexture::Create(L"../Bin/Resource/Texture/Grass/Brush%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_MinimapT",
			CTexture::Create(L"../Bin/Resource/Texture/MiniMap_T/AnyConv.com__WorldMap%d.png", 11, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Zoom",
			CTexture::Create(L"../Bin/Resource/Texture/Zoom/Aim%d.png", 2, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Water",
			CTexture::Create(L"../Bin/Resource/Texture/Water/Water%d.jpg", 2, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Fillter",
			CTexture::Create(L"../Bin/Resource/Texture/Terrain/Fillter%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass_Mix",
			CTexture::Create(L"../Bin/Resource/Texture/Terrain/Grass_Rocky%d.jpg", 2, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Ground",
			CTexture::Create(L"../Bin/Resource/Texture/Terrain/Ground%d.jpg", 2, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Castle",
			CTexture::Create(L"../Bin/Resource/Texture/Castle/Castle%d.jpg", 4, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_LastNumber",
			CTexture::Create(L"../Bin/Resource/Texture/LastNum/LastNumber%d.png", 6, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ScoreBack",
			CTexture::Create(L"../Bin/Resource/Texture/LastNum/Back%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hit",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/Hit/%d.png", 16, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Icon",
			CTexture::Create(L"../Bin/Resource/Texture/Icon/Icon%d.png", 5, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Gold",
			CTexture::Create(L"../Bin/Resource/Texture/Gold/Gold%d.png", 4, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_GoldNum",
			CTexture::Create(L"../Bin/Resource/Texture/num/num%d.png", 10, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_PopUp",
			CTexture::Create(L"../Bin/Resource/Texture/UI/popup%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Slot_Frame",
			CTexture::Create(L"../Bin/Resource/Texture/UI/slot_frame%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
	


		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_GameOver",
			CTexture::Create(L"../Bin/Resource/Texture/Gameover/GameOver%d.png", 27, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Win",
			CTexture::Create(L"../Bin/Resource/Texture/Win/Win%d.png", 14, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
	}

	//TGA
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Skill",
			CTexture::Create(L"../Bin/Resource/Texture/Icon/sIcon_0%d.tga", 5, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass",
			CTexture::Create(L"../Bin/Resource/Texture/Grass/Grass_%d.tga", 2, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_LowPolyTex",
			CTexture::Create(L"../Bin/Resource/Mesh/Static/LowPoly/rpgpp_lt_tex_a%d.tga", 1, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Human",
			CTexture::Create(L"../Bin/Resource/Mesh/Dynamic/Human/WK_StandardUnits%d.tga", 8, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Undead",
			CTexture::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/UD_Standard_Units%d.tga", 8, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Horse",
			CTexture::Create(L"../Bin/Resource/Mesh/Dynamic/Human/WK_Horse%d.tga", 4, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_NGround",
			CTexture::Create(L"../Bin/Resource/Texture/Terrain/NGround%d.tga", 3, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Icon_Class",
			CTexture::Create(L"../Bin/Resource/Texture/Icon/Icon_%d.tga", 9, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_MiniMap",
			CTexture::Create(L"../Bin/Resource/Texture/MiniMap_T/WorldMap%d.tga", 11, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Flag",
			CTexture::Create(L"../Bin/Resource/Texture/Flag/Flag%d.tga", 1, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Deer",
			CTexture::Create(L"../Bin/Resource/Texture/Animal/Deer%d.tga", 2, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Wolf",
			CTexture::Create(L"../Bin/Resource/Texture/Animal/Wolf%d.tga", 2, TEXTURE_TYPE::TEXTURE_TGA))))
			return E_FAIL;
	}

	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Shader(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_SkyBox",
		CShader::Create(L"../ShaderFiles/Shader_SkyBox.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain",
		CShader::Create(L"../ShaderFiles/Shader_Terrain.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Default",
		CShader::Create(L"../ShaderFiles/Shader_Default.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Texture",
		CShader::Create(L"../ShaderFiles/Shader_Texture.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Decal",
		CShader::Create(L"../ShaderFiles/Shader_Decal.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Merge_Deffered",
		CShader::Create(L"../ShaderFiles/Shader_Merge_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon",
		CShader::Create(L"../ShaderFiles/Shader_Toon.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI",
		CShader::Create(L"../ShaderFiles/Shader_UI.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Interface",
		CShader::Create(L"../ShaderFiles/Shader_UI_Interface.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Skill",
		CShader::Create(L"../ShaderFiles/Shader_UI_Skill.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_OnHead_Gage",
		CShader::Create(L"../ShaderFiles/Shader_OnHead_Gage.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Light",
		CShader::Create(L"../ShaderFiles/Shader_UI_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_InGame_UI",
		CShader::Create(L"../ShaderFiles/Shader_InGame_UI.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Fire",
		CShader::Create(L"../ShaderFiles/Shader_Fire.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_FireWall",
		CShader::Create(L"../ShaderFiles/Shader_FireWall.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching",
		CShader::Create(L"../ShaderFiles/Shader_Hatch.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Mesh_Default",
		CShader::Create(L"../ShaderFiles/Shader_Mesh_Default.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_OnHead",
		CShader::Create(L"../ShaderFiles/Shader_OnHead.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Water",
		CShader::Create(L"../ShaderFiles/Shader_Water.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Font",
		CShader::Create(L"../ShaderFiles/Shader_Font.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Collider",
		CShader::Create(L"../ShaderFiles/Shader_Collider.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Select",
		CShader::Create(L"../ShaderFiles/Shader_UI_Select.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_LastTime",
		CShader::Create(L"../ShaderFiles/Shader_UI_LastTime.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_End",
		CShader::Create(L"../ShaderFiles/Shader_UI_End.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Bloom",
		CShader::Create(L"../ShaderFiles/Shader_Bloom.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	//Compute_Shader_Compile
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_1",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default_1.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_2",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default_2.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_3",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default_3.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_4",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default_4.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_HP",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_HP.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_MAX_TIME",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_MaxTime.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_WoL_Red",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_WoL_Red.hlsl", "CS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_WoL_Blue",
			CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_WoL_Blue.hlsl", "CS_Main"))))
			return E_FAIL;

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation",
			CShader::Create(L"../ShaderFiles/Shader_Animation.hlsl", "CS_Main"))))
			return E_FAIL;

	}

	//Particle
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Particle",
			CShader::Create(L"../ShaderFiles/Shader_Particle.hlsl", "VS_Main", "PS_Main", "GS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Particle",
			CShader::Create(L"../ShaderFiles/Shader_Particle.hlsl", "CS_Main"))))
			return E_FAIL;
	}

	//Shadow
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow",
			CShader::Create(L"../ShaderFiles/Shader_Shadow.hlsl", "VS_Main", "PS_Main"))))
			return E_FAIL;
	}
	//Effect
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_PostEffect",
			CShader::Create(L"../ShaderFiles/Shader_PostEffect.hlsl", "VS_Main", "PS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_PostEffect_Buffer",
			CShader::Create(L"../ShaderFiles/Shader_PostEffect_Buffer.hlsl", "VS_Main", "PS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Blur",
			CShader::Create(L"../ShaderFiles/Shader_Blur.hlsl", "VS_Main", "PS_Main"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Reflection",
			CShader::Create(L"../ShaderFiles/Shader_Reflection.hlsl", "VS_Main", "PS_Main"))))
			return E_FAIL;
	}

	//Tess
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain_Tess",
			CShader::Create(L"../ShaderFiles/Shader_Terrain_Tess.hlsl", "VS_Main", "PS_Main", nullptr, "DS_Main", "HS_Main"))))
			return E_FAIL;
	}

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
	pManagement->Delete_All_Font();


	Safe_Release(pManagement);
	Safe_Release(m_pInput);
	CScene::Free();
}
