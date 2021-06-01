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
#include "Sphere.h"
// Mesh
#include "Orc01.h"
#include "Orc02.h"
#include "Orc03.h"
#include "Orc04.h"

#include "Skeleton.h"
#include "Sufferer.h"

// UI
#include "UI_Loading.h"
#include "UI_HP.h"
#include "UI_MP.h"
#include "UI_WoL.h"
#include "UI_WoL_Blue.h"

// Environment
#include "Fire.h"

#include "TestCube.h"
#include "TestMesh.h"

//Particle
#include "Particle_Default.h"

#include "Weapon01.h"
#include "Weapon02.h"
#include "Weapon03.h"
#include "Weapon04.h"

#include "TestAnimMesh.h"
#include "TestHatchMesh.h"
#include "TestMesh.h"

#include "Hatch.h"
#include "Building.h"
#include "LowPoly.h"
#include "Flag.h"


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


	if (FAILED(pManagement->Load_File(L"../Data/Demo/Fence00.dat")))
		return E_FAIL;
	if (FAILED(pManagement->Load_File_Low(L"../Data/Demo/Low.dat")))
		return E_FAIL;
	if (FAILED(pManagement->Load_File_Hatch(L"../Data/Demo/Hatch.dat")))
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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rect", CMyRect::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Sphere", CShpere::Create())))
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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Orc04", COrc04::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cube_Texture", CCube_Texture::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_HP", CUI_HP::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_MP", CUI_MP::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_WoL_Red", CUI_WoL::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_WoL_Blue", CUI_WoL_Blue::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fire", CFire::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestMesh", CTestMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestMeshHatch", CTestHatchMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TestAnim", CTestAnimMesh::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Particle_Default", CParticle_Default::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon01", CWeapon01::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon02", CWeapon02::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon03", CWeapon03::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Weapon04", CWeapon04::Create())))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Building", CBuilding::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_LowPoly", CLowPoly::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Hatch", CHatch::Create())))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Skeleton", CSkeleton::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Sufferer", CSufferer::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flag", CFlag::Create())))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage::Ready_Layer(CManagement* pManagement)
{
 	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Test(L"Layer_Test", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Debug_Camera(L"Layer_Camera", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Terrain_Height(L"Layer_Terrain", pManagement)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Deffered_UI(L"Layer_Deffered_UI", pManagement)))
		return E_FAIL;
	//if (FAILED(Ready_Layer_Environment(L"Layer_Environment", pManagement)))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Orc02(L"Layer_Orc02", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Orc03(L"Layer_Orc03", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_Flag(L"Layer_Flag", pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(L"Layer_UI", pManagement)))
		return E_FAIL;


	//if(FAILED(Ready_Layer_Castle(L"Layer_Castle", pManagement)))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Orc04(L"Layer_Orc04", pManagement)))

	//if (FAILED(Ready_Layer_Orc02(L"Layer_Orc02", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Orc03(L"Layer_Orc03", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Flag(L"Layer_Flag", pManagement)))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_UI(L"Layer_UI", pManagement)))
	//	return E_FAIL;

	////if (FAILED(Ready_Layer_Orc04(L"Layer_Orc04", pManagement)))
	////	return E_FAIL;
	//if (FAILED(Ready_Layer_Weapon(L"Layer_Weapon", pManagement)))
	//	return E_FAIL;



	// ¼­¹ö init

	return S_OK;
}

HRESULT CScene_Stage::Ready_Light(CManagement* pManagement)
{
	LIGHT	tLightInfo = {};
	ZeroMemory(&tLightInfo, sizeof(LIGHT));
	tLightInfo.iLightType = (_uint)LIGHT_TYPE::LIGHT_DIRECTIONAL;
	tLightInfo.tLightColor.vDiffuse = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vSpecular = _vec4(1.f, 1.f, 1.f, 0.f);
	tLightInfo.tLightColor.vAmbient = _vec4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.vLightDir = _vec4(-1.f, -1.f, 1.f, 0.f);
	tLightInfo.vLightPos = _vec4(250.f, 50.f, 250.f, 1.f);
	tLightInfo.fRange = 100000.f;
	if (FAILED(pManagement->Add_LightInfo(tLightInfo)))
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

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_HP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MP", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;	
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_WoL_Red", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_WoL_Blue", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
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
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Weapon02", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Weapon03", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Flag(const _tchar* pLayerTag, CManagement* pManagement)
{

	FLAG tFlag;
	//tFlag.vPos = _vec3(250.f, 0.2f, 250.f);
	tFlag.vPos = _vec3(50.f, 0.2f, 50.f);
	tFlag.iNum = 0;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;


	tFlag.vPos = _vec3(250.f, 0.2f, 250.f);
	tFlag.iNum = 1;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;



	tFlag.vPos = _vec3(450.f, 0.2f, 400.f);
	tFlag.iNum = 2;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;

	tFlag.vPos = _vec3(450.f, 0.2f, 100.f);
	tFlag.iNum = 3;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;


	tFlag.vPos = _vec3(100.f, 0.2f, 450.f);
	tFlag.iNum = 4;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, (void*)&tFlag)))
		return E_FAIL;



	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Test(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TestMesh", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc02(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc02", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, nullptr, 1)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc03(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc03", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Orc04(const _tchar* pLayerTag, CManagement* pManagement)
{
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc04", (_uint)SCENEID::SCENE_STAGE, pLayerTag)))
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
