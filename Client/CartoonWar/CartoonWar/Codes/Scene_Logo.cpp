#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"
// GameObject
#include "Logo.h"
// Defferd UI
#include "UI_Diffuse.h"
#include "UI_Normal.h"
#include "UI_Main.h"
#include "UI_Shade.h"
#include "UI_Specular.h"
#include "UI_Reflect.h"
#include "UI_PointLight.h"
#include "UI_Depth.h"
// New Scene
#include "Scene_Stage.h"


unsigned __stdcall ResourceLoadThread(void* pArguments)
{
	//Mesh Load Thread
	CScene_Logo* pLogo = reinterpret_cast<CScene_Logo*>(pArguments);
	EnterCriticalSection(&(pLogo->m_tCritical_Section));

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
	{
		LeaveCriticalSection(&(pLogo->m_tCritical_Section));
		return 0;
	}

	pManagement->AddRef();

	//pLogo->Ready_Add_Prototype_Mesh(pManagement);

	Safe_Release(pManagement);

	LeaveCriticalSection(&(pLogo->m_tCritical_Section));
	return 0;
}

CScene_Logo::CScene_Logo()
{
}

HRESULT CScene_Logo::Ready_Scene()
{
	m_eSceneID = SCENEID::SCENE_LOGO;
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
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		WaitForSingleObject(m_hThread_Handle, INFINITE);
		CloseHandle(m_hThread_Handle);
		DeleteCriticalSection(&m_tCritical_Section);

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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Diffuse", CUI_Diffuse::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Normal", CUI_Normal::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Main", CUI_Main::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Shade", CUI_Shade::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Specular", CUI_Specular::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Reflect", CUI_Reflect::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_PointLight", CUI_PointLight::Create())))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_Depth", CUI_Depth::Create())))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_Component(CManagement* pManagement)
{
	if (FAILED(Ready_Add_Prototype_Shader(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Buffer(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Texture(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Function(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_Texture_Mesh(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Add_Prototype_NaviMesh(pManagement)))
		return E_FAIL;	
 	if (FAILED(Ready_Add_Prototype_Mesh(pManagement)))
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
		CBuffer_Terrain::Create(100, 100))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height",
		CBuffer_Terrain_Height::Create(L"../Bin/Resource/Texture/Height/Height.bmp"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Sphere",
		CBuffer_Sphere::Create())))
		return E_FAIL;
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Function(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum", CFrustum::Create())))
		return E_FAIL;
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
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Texture_Mesh(CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc_01",
	//	m_pTextureCom = CTexture::Create(L"Texture_Orc_01", L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Armors_Albedo.tga"))))
	//	return E_FAIL;
	//if (nullptr == m_pTextureCom)
	//	return E_FAIL;
	//m_pTextureCom->AddRef();

	//if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc_01",
	//	L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Body_Albedo.tga")))
	//	return E_FAIL;
	//if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc_01",
	//	L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Armors_Normals.tga")))
	//	return E_FAIL;
	//if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc_01",
	//	L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Body_Normals.tga")))
	//	return E_FAIL;


	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_NaviMesh(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test",
		CNavigation::Create(L"../Data/TestNaviMesh.dat"))))
		return E_FAIL;
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Mesh(CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01",
	//	CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock01.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_A",
	//	CMesh::Create(L"../Bin/Resource/Mesh/Static/House/Monster.fbx"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc01",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Mesh/Orc_01_Mesh.FBX"))))
	//	return E_FAIL;


	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_A",
	//	CMesh::Create(L"../Bin/Resource/Mesh/Static/Rock/MountainRocks01_A.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_B",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks01_B.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_C",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks01_C.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock01_D",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks01_D.FBX"))))
	//	return E_FAIL;
	//
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks02.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02_A",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks02_A.FBX"))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock02_B",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainRocks02_B.FBX"))))
	//	return E_FAIL;
	//
	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Rock03",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock/MountainsRocks03.FBX"))))
	//	return E_FAIL;


	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Stone",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Rock01.FBX"))))
	//	return E_FAIL;
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Texture(CManagement* pManagement)
{
	//DDS
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Bricks",
		CTexture::Create(L"../Bin/Resource/Texture/Bricks/bricks%d.dds", 3, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
		return E_FAIL;
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

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch_123",
		CTexture::Create(L"../Bin/Resource/Texture/Hatching/Hatch123_%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch_456",
		CTexture::Create(L"../Bin/Resource/Texture/Hatching/Hatch456_%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS))))
		return E_FAIL;

	//PNG
	//	if(FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Logo",
	//	CTexture::Create(L"../Bin/Resource/Texture/Logo/Logo%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
	//	return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Noise",
		CTexture::Create(L"../Bin/Resource/Texture/Noise/Noise%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Particle_Smoke",
		CTexture::Create(L"../Bin/Resource/Texture/Particle/CartoonSmoke%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
		return E_FAIL;

	//TGA
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass",
		CTexture::Create(L"../Bin/Resource/Texture/Grass/Grass_%d.tga", 1, TEXTURE_TYPE::TEXTURE_TGA))))
		return E_FAIL;


	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Shader(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Default",
		CShader::Create(L"../ShaderFiles/Shader_Default.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Texture",
		CShader::Create(L"../ShaderFiles/Shader_Texture.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_SkyBox",
		CShader::Create(L"../ShaderFiles/Shader_SkyBox.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain",
		CShader::Create(L"../ShaderFiles/Shader_Terrain.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon",
		CShader::Create(L"../ShaderFiles/Shader_Toon.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Deffered",
		CShader::Create(L"../ShaderFiles/Shader_UI_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Merge_Deffered",
		CShader::Create(L"../ShaderFiles/Shader_Merge_Deffered.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Reflect",
		CShader::Create(L"../ShaderFiles/Shader_Reflect.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI",
		CShader::Create(L"../ShaderFiles/Shader_UI.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Environment",
		CShader::Create(L"../ShaderFiles/Shader_Environment.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Fire",
		CShader::Create(L"../ShaderFiles/Shader_Fire.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Light",
		CShader::Create(L"../ShaderFiles/Shader_UI_Light.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching",
		CShader::Create(L"../ShaderFiles/Shader_Hatch.hlsl", "VS_Main", "PS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default",
		CShader::Create(L"../ShaderFiles/Compute/Shader_Compute_Default.hlsl", "CS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Particle",
		CShader::Create(L"../ShaderFiles/Shader_Compute_Particle.hlsl", "CS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Particle",
		CShader::Create(L"../ShaderFiles/Shader_Particle.hlsl", "VS_Main", "PS_Main", "GS_Main"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation",
		CShader::Create(L"../ShaderFiles/Shader_Animation.hlsl", "CS_Main"))))
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


	//Safe_Release(m_pTextureCom);
	CScene::Free();
}
