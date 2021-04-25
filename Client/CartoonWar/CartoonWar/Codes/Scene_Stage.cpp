#include "framework.h"
#include "Scene_Stage.h"
#include "Management.h"

// Shape
#include "MyRect.h"
#include "Cube.h"
#include "Cube_Texture.h"
#include "SkyBox.h"
#include "Debug_Camera.h"
#include "Terrain.h"
#include "Terrain_Height.h"
// Mesh
#include "Orc01.h"
#include "Orc02.h"
#include "Orc03.h"

#include "Rock01_A.h"
// UI
#include "UI_Loading.h"
#include "UI_HP.h"
#include "UI_MP.h"
// Environment
#include "Fire.h"

#include "TestCube.h"
#include "TestMesh.h"

//Particle
#include "Particle_Default.h"

#include "Rock01.h"
#include "Rock01_A.h"
#include "Rock01_B.h"
#include "Rock01_C.h"
#include "Rock01_D.h"

#include "Rock02.h"
#include "Rock02_A.h"
#include "Rock02_B.h"

#include "Rock03.h"


#include "Bush0.h"
#include "Bush1.h"

#include "Cloud0.h"
#include "Cloud1.h"

#include "Flower0.h"
#include "Flower1.h"
#include "Flower2.h"

#include "Plant0.h"
#include "Plant1.h"

#include "Rock0.h"
#include "Rock1.h"
#include "Rock2.h"
#include "Rock3.h"
#include "Rock4.h"
#include "Rock5.h"

#include "Tree0.h"
#include "Tree1.h"
#include "Tree2.h"
#include "Tree3.h"
#include "Tree4.h"

#include "Weapon01.h"
#include "Weapon02.h"
#include "Weapon03.h"
#include "Weapon04.h"

#include "Castle0.h"

#include "TestAnimMesh.h"
#include "TestHatchMesh.h"
#include "TestMesh.h"


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


	//if (FAILED(pManagement->Load_File(L"../Data/Demo/Map_Demo.dat")))
	//	return E_FAIL;


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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rect", CMyRect::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cube", CCube::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestCube", CTestCube::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Camera_Debug", CDebug_Camera::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain", CTerrain::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain_Height", CTerrain_Height::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Orc01", COrc01::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Orc02", COrc02::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Orc03", COrc03::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cube_Texture", CCube_Texture::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_HP", CUI_HP::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_MP", CUI_MP::Create())))
		return E_FAIL;
	if(FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fire", CFire::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestMesh", CTestMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestMeshHatch", CTestHatchMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestAnim", CTestAnimMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Particle_Default", CParticle_Default::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks01", CRock01::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks01_A", CRock01_A::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks01_B", CRock01_B::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks01_C", CRock01_C::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks01_D", CRock01_D::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks02", CRock02::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks02_A", CRock02_A::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks02_B", CRock02_B::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_MountainRocks03", CRock03::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_tree_1", CRock03::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_tree_01", CTree0::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_tree_02", CTree1::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_tree_pine_01", CTree2::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_flower_01", CFlower0::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_flower_02", CFlower1::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_flower_03", CFlower2::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_plant_01", CPlant0::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_plant_02", CPlant1::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_rock_01", CRock0::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_rock_02", CRock1::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_rock_03", CRock2::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_rock_small_01", CRock3::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_rock_small_02", CRock4::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_bush_01", CBush0::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_rpgpp_lt_bush_02", CBush1::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon01", CWeapon01::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon02", CWeapon02::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon03", CWeapon03::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon04", CWeapon04::Create())))
		return E_FAIL; 

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Castle0", CCastle0::Create())))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage::Ready_Layer(CManagement* pManagement)
{
	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox", pManagement)))
		return E_FAIL;
	//if (FAILED(Ready_Layer_Test(L"Layer_Test", pManagement)))
	//	return E_FAIL;
	if (FAILED(Ready_Layer_Debug_Camera(L"Layer_Camera", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Terrain_Height(L"Layer_Terrain", pManagement)))
		return E_FAIL;
	//if (FAILED(Ready_Layer_UI(L"Layer_UI", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_BasicShape(L"Layer_BasicShape", pManagement)))
	//	return E_FAIL;
	if (FAILED(Ready_Layer_Deffered_UI(L"Layer_Deffered_UI", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Environment(L"Layer_Environment", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Orc02(L"Layer_Orc02", pManagement)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Orc03(L"Layer_Orc03", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Weapon(L"Layer_Weapon", pManagement)))
		return E_FAIL;

	//if(FAILED(Ready_Layer_Castle(L"Layer_Castle", pManagement)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Light(CManagement* pManagement)
{
	LIGHT	tLightInfo = {};
	ZeroMemory(&tLightInfo, sizeof(LIGHT));
	tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_DIRECTIONAL;
	tLightInfo.tLightColor.vDiffuse = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vSpecular = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vAmbient = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	tLightInfo.vLightPos = _vec4(100.f, 0.f, 0.f, 1.f);
	tLightInfo.fRange = 100.f;
	if (FAILED(pManagement->Add_LightInfo(L"Light_Default", tLightInfo)))
		return E_FAIL;


	/*ZeroMemory(&tLightInfo, sizeof(LIGHT));
	tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	tLightInfo.tLightColor.vDiffuse = _vec4(1.f, 0.f, 0.f, 0.f);
	tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	tLightInfo.vLightPos = _vec4(15.f, 5.5f, 15.f, 1.f);
	tLightInfo.fRange = 15.f;
	if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Red", tLightInfo)))
		return E_FAIL;

	ZeroMemory(&tLightInfo, sizeof(LIGHT));
	tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	tLightInfo.tLightColor.vDiffuse = _vec4(1.f, 0.15f, 0.f, 0.f);
	tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	tLightInfo.vLightPos = _vec4(15.f, 5.5f, 15.f, 1.f);
	tLightInfo.fRange = 15.f;
	if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Red", tLightInfo)))
		return E_FAIL;*/

	//ZeroMemory(&tLightInfo, sizeof(LIGHT));
	//tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	//tLightInfo.tLightColor.vDiffuse = _vec4(0.15f, 0.15f, 1.f, 0.f);
	//tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	//tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	//tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	//tLightInfo.vLightPos = _vec4(30.f, 5.5f, 15.f, 1.f);
	//tLightInfo.fRange = 15.f;
	//if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Blue", tLightInfo)))
	//	return E_FAIL;

	//ZeroMemory(&tLightInfo, sizeof(LIGHT));
	//tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	//tLightInfo.tLightColor.vDiffuse = _vec4(0.15f, 1.f, 0.15f, 0.f);
	//tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	//tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	//tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	//tLightInfo.vLightPos = _vec4(22.5f, 5.5f, 22.5f, 1.f);
	//tLightInfo.fRange = 15.f;
	//if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Blue", tLightInfo)))
	//	return E_FAIL;

	//ZeroMemory(&tLightInfo, sizeof(LIGHT));
	//tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	//tLightInfo.tLightColor.vDiffuse = _vec4(0.15f, 1.f, 0.15f, 0.f);
	//tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	//tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	//tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	//tLightInfo.vLightPos = _vec4(22.5f, 5.5f, 22.5f, 1.f);
	//tLightInfo.fRange = 15.f;
	//if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Blue", tLightInfo)))
	//	return E_FAIL;


	//ZeroMemory(&tLightInfo, sizeof(LIGHT));
	//tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	//tLightInfo.tLightColor.vDiffuse = _vec4(0.15f, 0.15f, 1.f, 0.f);
	//tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	//tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	//tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	//tLightInfo.vLightPos = _vec4(30.f, 5.5f, 15.f, 1.f);
	//tLightInfo.fRange = 15.f;
	//if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Blue", tLightInfo)))
	//	return E_FAIL;



	//ZeroMemory(&tLightInfo, sizeof(LIGHT));
	//tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_POINT;
	//tLightInfo.tLightColor.vDiffuse = _vec4(0.15f, 1.f, 0.15f, 0.f);
	//tLightInfo.tLightColor.vSpecular = _vec4(1.f, 0.f, 0.f, 0.f);
	//tLightInfo.tLightColor.vAmbient = _vec4(0.3f, 0.3f, 0.3f, 0.f);
	//tLightInfo.vLightDir = _vec4(1.f, -1.f, 1.f, 0.f);
	//tLightInfo.vLightPos = _vec4(22.5f, 5.5f, 22.5f, 1.f);
	//tLightInfo.fRange = 15.f;
	//if (FAILED(pManagement->Add_LightInfo(L"Light_Point_Green", tLightInfo)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BasicShape(const _tchar* pLayerTag, CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Cube", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestCube", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
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

HRESULT CScene_Stage::Ready_Layer_Terrain_Height(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc(const _tchar* pLayerTag, CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_MountainRocks01_A", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_HP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Deffered_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	//GameObject_UI_Diffuse
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Diffuse", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Normal", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Shade", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Position", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_PointLight", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Main", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Environment(const _tchar* pLayerTag, CManagement* pManagement)
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

HRESULT CScene_Stage::Ready_Layer_Weapon(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Weapon01", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Test(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestAnim", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc02(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc02", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc03(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc03", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Castle(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Castle0", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
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
