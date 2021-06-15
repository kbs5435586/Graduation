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




unsigned __stdcall ResourceLoadThread(void* pArguments)
{
	//Mesh Load Thread
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
	//Mesh Load Thread
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
}

HRESULT CScene_Logo::Ready_Scene()
{
	m_eSceneID = SCENEID::SCENE_LOGO;
	InitializeCriticalSection(&m_tCritical_Section_Mesh);
	m_hThread_Handle_Mesh = (HANDLE)_beginthreadex(nullptr, 0, ResourceLoadThread, this, 0, nullptr);


	InitializeCriticalSection(&m_tCritical_Section_Shader);
	m_hThread_Handle_Shader = (HANDLE)_beginthreadex(nullptr, 0, ShaderCompileThread, this, 0, nullptr);

	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();



	WaitForSingleObject(m_hThread_Handle_Mesh, INFINITE);
	CloseHandle(m_hThread_Handle_Mesh);
	DeleteCriticalSection(&m_tCritical_Section_Mesh);


	WaitForSingleObject(m_hThread_Handle_Shader, INFINITE);
	CloseHandle(m_hThread_Handle_Shader);
	DeleteCriticalSection(&m_tCritical_Section_Shader);


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
	if (GetKeyState(VK_SPACE) & 0x8000)
	{

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



	//

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

	if (FAILED(Ready_Add_Prototype_NaviMesh(pManagement)))
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
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Normal", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Position", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Shade", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
	//	return E_FAIL;	
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_PointLight", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_Main", (_uint)SCENEID::SCENE_LOGO, pLayerTag)))
	//	return E_FAIL;
	return S_OK;
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
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain",
		CBuffer_Terrain::Create(500, 500))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height",
		CBuffer_Terrain_Height::Create(L"../Bin/Resource/Texture/Height/Height.bmp"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Sphere",
		CBuffer_Sphere::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Point",
		CBuffer_Point::Create())))
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


	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh",
		CNavigation::Create(L"../Data/Demo/Navi_Demo.dat"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Add_Prototype_NaviMesh(CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test",
	//	CNavigation::Create(L"../Data/Demo/Navi_Demo.dat"))))
	//	return E_FAIL;
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Mesh(CManagement* pManagement)
{

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
	}


	{
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

		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Flag_Blue",
			CMesh::Create_Load(L"../Data/MeshData/Flag_Blue.dat"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Flag_Red",
			CMesh::Create_Load(L"../Data/MeshData/Flag_Red.dat"))))
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
			return E_FAIL;*/


		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Cavalry",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Heavy_Cavalry.fbx", L"../Data/Undead/Heavy_Cavalry.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Infantry",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Heavy_Infantry.fbx", L"../Data/Undead/Heavy_Infantry.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Cavalry",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Light_Cavalry.fbx", L"../Data/Undead/Light_Cavalry.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Infantry",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Light_Infantry.fbx", L"../Data/Undead/Light_Infantry.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mage",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Mage.fbx", L"../Data/Undead/Mage.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mounted_Mage",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Mounted_Mage.fbx", L"../Data/Undead/Mounted_Mage.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Spearman",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Spearman.fbx", L"../Data/Undead/SpearMan.dat"))))
		//	return E_FAIL;
		//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Worker",
		//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Worker.fbx", L"../Data/Undead/Worker.dat"))))
		//	return E_FAIL;
	}
	{
	/*		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_build_barracks_01_low",
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

	//if(FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Infantry",
	//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Light_Infantry.fbx", L"../Data/Undead/Light_Infantry.dat"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_SpearMan",
	//	CMesh::Create(L"../Bin/Resource/Mesh/Dynamic/Undead/Undead_Spearman.fbx", L"../Data/Undead/SpearMan.dat"))))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Add_Prototype_Texture(CManagement* pManagement)
{
	//DDS 
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_SkyBox",
			CTexture::Create(L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS, true))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_FireAlpha",
			CTexture::Create(L"../Bin/Resource/Texture/Fire/Fire_alpha%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
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
	}

	//PNGJPG
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Logo",
			CTexture::Create(L"../Bin/Resource/Texture/Logo/Logo%d.jpg", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Noise",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/noise%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_CartoonSmoke",
			CTexture::Create(L"../Bin/Resource/Texture/Particle/CartoonSmoke%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
			return E_FAIL;
	}
	
	//TGA
	{
		if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass",
			CTexture::Create(L"../Bin/Resource/Texture/Grass/Grass_%d.tga", 1, TEXTURE_TYPE::TEXTURE_TGA))))
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
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Merge_Deffered",
		CShader::Create(L"../ShaderFiles/Shader_Merge_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon",
		CShader::Create(L"../ShaderFiles/Shader_Toon.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI",
		CShader::Create(L"../ShaderFiles/Shader_UI.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Light",
		CShader::Create(L"../ShaderFiles/Shader_UI_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Fire",
		CShader::Create(L"../ShaderFiles/Shader_Fire.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching",
		CShader::Create(L"../ShaderFiles/Shader_Hatch.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Mesh_Default",
		CShader::Create(L"../ShaderFiles/Shader_Mesh_Default.hlsl", "VS_Main", "PS_Main"))))
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
