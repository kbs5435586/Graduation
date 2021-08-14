#include "framework.h"
#include "Scene_Stage.h"
#include "Management.h"

#include "Player.h"
#include "Player_Inven.h"
#include "NPC.h"
#include "Throw_Arrow.h"


// Shape
#include "MyRect.h"
#include "Cube.h"
#include "Cube_Texture.h"
#include "SkyBox.h"
#include "Debug_Camera.h"
#include "Light_Camera.h"
#include "Inventory_Camera.h"
#include "Reflection_Camera.h"
#include "Map_Camera.h"
#include "Terrain.h"
#include "Terrain_Height.h"
#include "Sphere.h"
#include "MapUser.h"


// UI
#include "UI_Loading.h"
#include "UI_HP.h"
#include "UI_MP.h"
#include "UI_WoL.h"
#include "UI_WoL_Blue.h"
#include "UI_OnHead.h"
#include "UI_MiniMap.h"
#include "UI_Aim.h"
#include "UI_OnHead_Gage.h"
#include "UI_Score.h"
#include "UI_NormalBar.h"
#include "UI_ClassTap.h"
#include "UI_Skill.h"
#include "UI_Shop.h"
#include "UI_CharInterface.h"
#include "UI_Button.h"
#include "UI_ButtonNPC.h"

// Environment
#include "Fire.h"
//Particle
#include "Particle_Default.h"
#include "Particle_Run.h"

#include "TestAnimMesh.h"
#include "TestHatchMesh.h"
#include "TestMesh.h"
#include "TestBuffer.h"
#include "Test_Tess.h"


#include "Hatch.h"
#include "Building.h"
#include "LowPoly.h"
#include "Flag.h"
#include "Castle.h"
#include "Teleport.h"

#include "Water.h"
#include "Deffend.h"
#include "Animals.h"
#include "EffectBox.h"


CScene_Stage::CScene_Stage()
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	m_eSceneID = SCENEID::SCENE_STAGE;
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
	//if (FAILED(pManagement->Load_File_Hatch(L"../Data/Demo/Hatch.dat")))
	//	return E_FAIL;
	//if (FAILED(pManagement->Load_File_Castle(L"../Data/Castle/Castle.dat")))
	//	return E_FAIL;
	//
	// 
	



	if (FAILED(pManagement->Load_File(L"../Data/Map/LittleFence_Test1.dat")))
		return E_FAIL;
	ENVITYPE eEnviType = ENVITYPE::ENVI_TREE;
	if (FAILED(pManagement->Load_File_Low(L"../Data/Map/LittleTree.dat", (void*)&eEnviType)))
		return E_FAIL;
	////eEnviType = ENVITYPE::ENVI_FLOWER;
	////if (FAILED(pManagement->Load_File_Low(L"../Data/Map/FlowerTest.dat", (void*)&eEnviType)))
	////	return E_FAIL;
	////eEnviType = ENVITYPE::ENVI_PLANT;
	////if (FAILED(pManagement->Load_File_Low(L"../Data/Map/PlantTest.dat", (void*)&eEnviType)))
	////	return E_FAIL;
	eEnviType = ENVITYPE::ENVI_ROCK;
	if (FAILED(pManagement->Load_File_Low(L"../Data/Map/LittleRock.dat", (void*)&eEnviType) ))
		return E_FAIL;

	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return E_FAIL;

	server->AddRef();
	server->InitServer(g_hWnd);

	Safe_Release(server);
	g_IsCollisionStart = true;
	CManagement::GetInstance()->Stop_Sound(CHANNEL_BG);
	pManagement->Play_Sound(CHANNEL_BG, SOUND_BG, BG, 0.f, FMOD_LOOP_NORMAL);

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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CDebug_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Light", CLight_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Inventory", CInventory_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Reflection", CReflection_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Map", CMap_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain_Height", CTerrain_Height::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_HP", CUI_HP::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_OnHead", CUI_OnHead::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_OnHead_Gage", CUI_OnHead_Gage::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_MiniMap", CUI_MiniMap::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Aim", CUI_Aim::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Score", CUI_Score::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_NormalBar", CUI_NormalBar::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_ClassTap", CUI_ClassTap::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Shop", CUI_Shop::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_CharInterface", CUI_CharInterface::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Button", CUI_Button::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_ButtonNPC", CUI_ButtonNPC::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Skill", CUI_Skill::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fire", CFire::Create())))
		return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Skill_Fire", CSkill_Fire::Create())))
	//	return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Particle_Default", CParticle_Default::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Particle_Run", CParticle_Run::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Building", CBuilding::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_LowPoly", CLowPoly::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Hatch", CHatch::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flag", CFlag::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Player", CPlayer::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Animals", CAnimals::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Player_Inventory", CPlayer_Inven::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_NPC", CNPC::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_ThrowArrow", CThrow_Arrow::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Deffend", CDeffend::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Teleport", CTeleport::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Water", CWater::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_EffectBox", CEffectBox::Create())))
		return E_FAIL;
	return S_OK;
}


HRESULT CScene_Stage::Ready_Layer(CManagement* pManagement)
{
 	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Debug_Camera(L"Layer_Camera", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Terrain_Height(L"Layer_Terrain", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Deffered_UI(L"Layer_Deffered_UI", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Player(L"Layer_Player", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Light_Camera(L"Layer_Light_Camera", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_NPC(L"Layer_NPC", pManagement)))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Environment(L"Layer_Environment", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Reflection_Camera(L"Layer_Reflection_Camera", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Test(L"Layer_Test", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Particle(L"Layer_Particle", pManagement)))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Inventory(L"Layer_Inventory_Player", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Inventory_Camera(L"Layer_Inventory_Camera", pManagement)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Camera(L"Layer_Map_Camera", pManagement)))
		return E_FAIL;
	//if (FAILED(Ready_Layer_Map(L"Layer_Map_Camera", pManagement)))
	//	return E_FAIL;
	
	if (FAILED(Ready_Layer_SkillFire(L"Layer_SkillFire", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Flag(L"Layer_Flag", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(L"Layer_UI", pManagement)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Light(CManagement* pManagement)
{
	LIGHT	tLightInfo = {};
	ZeroMemory(&tLightInfo, sizeof(LIGHT));
	tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_DIRECTIONAL;
	tLightInfo.tLightColor.vDiffuse = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vSpecular = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f,0.f);
	tLightInfo.vLightPos = _vec4(250.f, 50.f, 250.f, 1.f);
	tLightInfo.fRange = 100000.f;
	if (FAILED(pManagement->Add_LightInfo(tLightInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BasicShape(const _tchar* pLayerTag, CManagement* pManagement)
{


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
	tCameraDesc.vEye = _vec3(0.f, 0.f, 0.f);
	tCameraDesc.vAt = _vec3(0.f, 0.f, 1.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);
	PROJDESC		tProjDesc;
	ZeroMemory(&tProjDesc, sizeof(tProjDesc));
	tProjDesc.fFovY = XMConvertToRadians(60.f);
	tProjDesc.fAspect = _float(WINCX) / WINCY;
	tProjDesc.fNear = g_Near;
	tProjDesc.fFar = 500.f;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Light_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{
	CLight_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Light", (_uint)SCENEID::SCENE_STAGE, pLayerTag,
		(CGameObject**)&pCameraObject)))
		return E_FAIL;

	CAMERADESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERADESC));
	tCameraDesc.vEye = _vec3(-1000.f, 1000.f, -1000.f);
	tCameraDesc.vAt = _vec3(1.f, -1.f, 1.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);

	PROJDESC		tProjDesc;
	ZeroMemory(&tProjDesc, sizeof(tProjDesc));
	tProjDesc.fFovY = XMConvertToRadians(60.f);
	tProjDesc.fAspect = _float(WINCX) / WINCY;
	tProjDesc.fNear = g_Near;
	tProjDesc.fFar = 300.f;


	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc, true)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Inventory_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{
	CInventory_Camera* pICameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Inventory", (_uint)SCENEID::SCENE_STAGE, pLayerTag,
		(CGameObject**)&pICameraObject)))
		return E_FAIL;

	CAMERADESC		tICameraDesc;
	ZeroMemory(&tICameraDesc, sizeof(CAMERADESC));
	tICameraDesc.vEye = _vec3(0.f, 0.f, 0.f);
	tICameraDesc.vAt = _vec3(0.f, 0.f, 1.f);
	tICameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);

	PROJDESC		tIProjDesc;
	ZeroMemory(&tIProjDesc, sizeof(tIProjDesc));
	tIProjDesc.fFovY = XMConvertToRadians(60.f);
	tIProjDesc.fAspect = _float(WINCX) / WINCY;
	tIProjDesc.fNear = g_Near;
	//tIProjDesc.fFar = g_Far;
	tIProjDesc.fFar = 100.f;
	_float aaa = 1;

	if (FAILED(pICameraObject->SetUp_CameraProjDesc(tICameraDesc, tIProjDesc, 1)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Reflection_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{
	CReflection_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Reflection", (_uint)SCENEID::SCENE_STAGE, pLayerTag,
		(CGameObject**)&pCameraObject)))
		return E_FAIL;

	CAMERADESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERADESC));
	tCameraDesc.vEye = _vec3(50.f, -10.f, 50.f);
	tCameraDesc.vAt = _vec3(0.f, 0.f, 1.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, -0.f);
	PROJDESC		tProjDesc;
	ZeroMemory(&tProjDesc, sizeof(tProjDesc));
	tProjDesc.fFovY = XMConvertToRadians(60.f);
	tProjDesc.fAspect = _float(WINCX) / WINCY;
	tProjDesc.fNear = g_Near;
	tProjDesc.fFar = 11300.f;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc, 1)))
		return E_FAIL;



	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Map_Camera(const _tchar* pLayerTag, CManagement* pManagement)
{
	CDebug_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Map", (_uint)SCENEID::SCENE_STAGE, pLayerTag,
		(CGameObject**)&pCameraObject)))
		return E_FAIL;
	//130.f,100.f,300.f
	CAMERADESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERADESC));
	tCameraDesc.vEye = _vec3(130.f, 160.f, 300.f);
	//tCameraDesc.vEye = _vec3(0.f, 0.f, 0.f);
	tCameraDesc.vAt = _vec3(130.f, 150.f, 301.f);
	//tCameraDesc.vAt = _vec3(0.f, 0.f, 1.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);
	PROJDESC		tProjDesc;
	ZeroMemory(&tProjDesc, sizeof(tProjDesc));
	tProjDesc.fFovY = XMConvertToRadians(60.f);
	tProjDesc.fAspect = _float(WINCX) / WINCY;
	tProjDesc.fNear = g_Near;
	tProjDesc.fFar = 1500.f;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc, (_short)1)))
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

HRESULT CScene_Stage::Ready_Layer_Terrain_Height(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain_Height", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	// 0 샵 버튼
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Shop", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	// 1 캐릭터 인터페이스
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_CharInterface", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	_float m_fX = WINCX / 2;
	_float m_fY = WINCY / 2;
	_vec2 m_Pos[9] = { {m_fX - 250,465},{ m_fX - 380,525},{m_fX - 380,590},{m_fX - 250,525},
		{m_fX - 250,590} ,{m_fX - 315,525} ,{m_fX - 120,525},{m_fX - 120,590} ,{m_fX - 185,525} };

	// 2 - 10 클래스 버튼
	for (int i = 0; i < 9; ++i)
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Button", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&m_Pos[i])))
			return E_FAIL;
	}
	// 11 - 20 NPC 버튼
	for (int i = 0; i < 10; ++i)
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_ButtonNPC", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
			return E_FAIL;
	}

	// 21 클래스 탭
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_ClassTap", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	

	XMFLOAT3 one = { 560.f, 100.f, 0.f };
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Skill", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&one)))
		return E_FAIL; 
	XMFLOAT3 two = { 405.f, 100.f, 1.f };
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Skill", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&two)))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MiniMap", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_HP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Aim", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	_bool IsRed = true;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Score", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&IsRed)))
		return E_FAIL;
	IsRed = false;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Score", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&IsRed)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_NormalBar", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr)))
		return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;	
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_WoL_Red", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_WoL_Blue", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Deffered_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	//GameObject_UI_Diffuse
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Diffuse", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Normal", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Shade", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Position", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_PointLight", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	_bool a = true;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Main", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&a)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Environment(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Water", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_SkillFire(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Fire", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Particle(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Particle_Default", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Flag(const _tchar* pLayerTag, CManagement* pManagement)
{
	FLAG tFlag;
	tFlag.vPos = _vec3(392, 0.2f, 471);
	tFlag.iNum = 0;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos, tFlag.iNum)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead_Gage", (_uint)SCENEID::SCENE_STAGE, L"Layer_Flag_OnHead_UI", nullptr, (void*)&tFlag.iNum, tFlag.iNum)))
		return E_FAIL;

	tFlag.vPos = _vec3(385, 0.2f, 3036.53f);
	tFlag.iNum = 1;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos, tFlag.iNum)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead_Gage", (_uint)SCENEID::SCENE_STAGE, L"Layer_Flag_OnHead_UI", nullptr, (void*)&tFlag.iNum, tFlag.iNum)))
		return E_FAIL;

	tFlag.vPos = _vec3(1626.96f, 0.2f, 2352.42f);
	tFlag.iNum = 2;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos, tFlag.iNum)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead_Gage", (_uint)SCENEID::SCENE_STAGE, L"Layer_Flag_OnHead_UI", nullptr, (void*)&tFlag.iNum, tFlag.iNum)))
		return E_FAIL;

	tFlag.vPos = _vec3(3249.09f, 0.2f, 3419.41f);
	tFlag.iNum = 3;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos, tFlag.iNum)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead_Gage", (_uint)SCENEID::SCENE_STAGE, L"Layer_Flag_OnHead_UI", nullptr, (void*)&tFlag.iNum, tFlag.iNum)))
		return E_FAIL;

	tFlag.vPos = _vec3(2183.09f, 0.2f, 646.43f);
	tFlag.iNum = 4; 
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos, tFlag.iNum)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead_Gage", (_uint)SCENEID::SCENE_STAGE, L"Layer_Flag_OnHead_UI", nullptr, (void*)&tFlag.iNum, tFlag.iNum)))
		return E_FAIL;

	//tFlag.vPos = _vec3(50, 0.2f, 50);
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Teleport", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag.vPos)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Test(const _tchar* pLayerTag, CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestBuffer", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	for (int i = 0; i < 100; ++i)
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestMesh", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Player(const _tchar* pLayerTag, CManagement* pManagement)
{
	UNIT tInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_RED };
	ORDER tOrder = ORDER();
	tOrder.IsPlayer = true;
	tOrder.iIdx = 0;
	tOrder.iIdx = 0;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
		return E_FAIL;

	tInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_BLUE };
	tOrder = ORDER();
	tOrder.IsPlayer = true;
	tOrder.iIdx = 1;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
		return E_FAIL;

	tInfo = { SPECIES::SPECIES_UNDEAD, COLOR::COLOR_BLACK };
	tOrder = ORDER();
	tOrder.IsPlayer = true;
	tOrder.iIdx = 2;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
		return E_FAIL;

	tInfo = { SPECIES::SPECIES_UNDEAD, COLOR::COLOR_WHITE };
	tOrder = ORDER();
	tOrder.IsPlayer = true;
	tOrder.iIdx = 3;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
		return E_FAIL;

	tOrder = ORDER();
	tOrder.IsPlayer = true;
	tOrder.iIdx = 2;
	tInfo = { SPECIES::SPECIES_UNDEAD, COLOR::COLOR_GREEN };
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Inventory(const _tchar* pLayerTag, CManagement* pManagement)
{
	UNIT tPlayerInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_RED };

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Player_Inventory", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tPlayerInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_NPC(const _tchar* pLayerTag, CManagement* pManagement)
{
	for (int i = 0; i < CL_TEST_NPCS; ++i)
	{
		ORDER tOrder = ORDER();
		tOrder.IsPlayer = false;
		tOrder.iIdx = i;
		if (i < CL_NPC_TEAM1)
		{
			UNIT tInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_RED };
			if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_NPC", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
				return E_FAIL;
		}
		else if (i >= CL_NPC_TEAM1 && i < CL_NPC_TEAM2)
		{
			UNIT tInfo = { SPECIES::SPECIES_HUMAN, COLOR::COLOR_BLUE };
			if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_NPC", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
				return E_FAIL;
		}
		else if (i >= CL_NPC_TEAM2 && i < CL_NPC_TEAM3)
		{
			UNIT tInfo = { SPECIES::SPECIES_UNDEAD, COLOR::COLOR_BLACK };
			if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_NPC", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
				return E_FAIL;
		}
		else if (i >= CL_NPC_TEAM3 && i < CL_NPC_TEAM4)
		{
			UNIT tInfo = { SPECIES::SPECIES_UNDEAD, COLOR::COLOR_WHITE };
			if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_NPC", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tInfo, tOrder.iIdx)))
				return E_FAIL;
		}
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_OnHead", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI_OnHead", nullptr, (void*)&tOrder)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Map(const _tchar* pLayerTag, CManagement* pManagement)
{
	//for(auto iter)
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestBuffer", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
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
