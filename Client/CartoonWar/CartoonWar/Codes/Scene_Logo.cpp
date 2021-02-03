#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"
// GameObject
#include "Logo.h"
// New Scene
#include "Scene_Stage.h"

unsigned __stdcall ResourceLoadThread(void* pArguments)
{
	//Mesh Load Thread
	CScene_Logo* pLogo = reinterpret_cast<CScene_Logo*>(pArguments);
	EnterCriticalSection(&(pLogo->m_tCritical_Section));

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return 0;

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
	CDevice::GetInstance()->GetCmdQueue().Get();
	CDevice::GetInstance()->GetRenderTarget().Get();



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
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer(CManagement* pManagement)
{
	if (FAILED(Ready_Layer_Logo(L"Layer_Logo", pManagement)))
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
		CBuffer_Terrain::Create(100,100))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height",
		CBuffer_Terrain_Height::Create(L"../Bin/Resource/Texture/Height/Height.bmp"))))
		return E_FAIL;

	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Function(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum", CFrustum::Create())))
		return E_FAIL;
	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Texture_Mesh(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc",
		m_pTextureCom=CTexture::Create(L"Texture_Orc", L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Armors_Albedo.tga"))))
		return E_FAIL;
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	m_pTextureCom->AddRef();

	if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc", 
		L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Body_Albedo.tga")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc",
		L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/T_Hair_Albedo.tga")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc",
		L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Armors_Normals.tga")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Ready_Texture(L"Texture_Orc",
		L"../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Textures/Orc_01_Body_Normals.tga")))
		return E_FAIL;


	return S_OK;
}
HRESULT CScene_Logo::Ready_Add_Prototype_Mesh(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc01",
		CStatic_Mesh::Create("../Bin/Resource/Mesh/Dynamic/Orc/Orc_01/Mesh/Orc_01_Mesh.FBX"))))
		return E_FAIL;

	//if (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc01",
	//	CStatic_Mesh::Create("../Bin/Resource/Mesh/Static/Monster/Monster.fbx"))))
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

	//PNG
	//f (FAILED(pManagement->Add_Prototype_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Logo",
	//	CTexture::Create(L"../Bin/Resource/Texture/Logo/Logo%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG))))
	//	return E_FAIL;


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
	Safe_Release(m_pTextureCom);
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return;

	pManagement->AddRef();

	pManagement->Clear_Layers((_uint)SCENEID::SCENE_LOGO);

	Safe_Release(pManagement);
	CScene::Free();
}
