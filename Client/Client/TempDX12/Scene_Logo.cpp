#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"
#include "Debug_Camera.h"

#include "Cube.h"
#include "Rect.h"
#include "TextureRect.h"
#include "Terrain.h"
#include "Texture_Terrain.h"
#include "Height_Terrain.h"

#include "TempMesh.h"

#include "TempStaticMesh.h"
#include "TempStaticMesh1.h"
#include "TempStaticMesh2.h"
#include "TempStaticMesh3.h"
#include "SkyBox.h"

#include "Light_Manager.h"


CScene_Logo::CScene_Logo(ID3D12Device* pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if(FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;


	if (FAILED(Ready_Layer_TempStatic_Mesh(L"Layer_TempStatic_Mesh")))
		return E_FAIL;

	if (FAILED(Ready_Layer_TextureRect(L"Layer_TextureRect")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain_Height(L"Layer_Terrain")))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox")))
		return E_FAIL;


	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Logo::Update_Scene(const _float& fTimeDelta)
{
	return CScene::Update_Scene(fTimeDelta);
}

_int CScene_Logo::LastUpdate_Scene(const _float& fTimeDelta)
{
	return CScene::LastUpdate_Scene(fTimeDelta);
}

void CScene_Logo::Render_Scene()
{
	int a = 10;
}

HRESULT CScene_Logo::Ready_Prototype_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cube", CCube::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rect", CRect::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TextureRect", CTextureRect::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain_Texture", CTexture_Terrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain_Height", CHeight_Terrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempMesh", CTempMesh::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempStatic_Mesh", CTempStaticMesh::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempStatic_Mesh1", CTempStaticMesh1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempStatic_Mesh2", CTempStaticMesh2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempStatic_Mesh3", CTempStaticMesh3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	
	if (FAILED(Add_Prototype_Component_Shader(pManagement)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Component_Buffer(pManagement)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Component_Texture(pManagement)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Component_Static_Mesh(pManagement)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Component_Dynamic_Mesh(pManagement)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Light()
{
	CLight_Manager* pLight_Manager = CLight_Manager::GetInstance();
	if (nullptr == pLight_Manager)
		return E_FAIL;
	pLight_Manager->AddRef();

	LIGHT	tLightInfo = {};
	ZeroMemory(&tLightInfo, sizeof(LIGHT));

	tLightInfo.eLightType = LIGHT_DIRECTIONAL;
	tLightInfo.vDiffuse = _vec4(1.f,1.f,1.f,1.f);
	tLightInfo.vSpecular = _vec4(1.f,1.f,1.f,1.f);
	tLightInfo.vAmbient = _vec4(1.f,1.f,1.f,1.f);

	tLightInfo.vDirection = _vec4(1.f,-1.f,1.f,0.f);

	if (FAILED(pLight_Manager->Add_LightInfo(m_pGraphic_Device, L"Light_Default",tLightInfo)))
		return E_FAIL;

	Safe_Release(pLight_Manager);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	CDebug_Camera* pCameraObject = nullptr;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Debug", SCENE_STAGE, pLayerTag, (CGameObject**)&pCameraObject)))
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
	tProjDesc.fNear = 0.2f;
	tProjDesc.fFar = 5000.f;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(tCameraDesc, tProjDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Cube(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Cube", SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Rect(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_TextureRect(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();
	_uint num = 0;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TextureRect",SCENE_LOGO, pLayerTag, nullptr, (void*)&num)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Terrain_Texture(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain_Texture", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Terrain_Height(const _tchar* pLayerTag)
{

	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain_Height", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Temp_Mesh(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TempMesh", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_TempStatic_Mesh(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TempStatic_Mesh", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TempStatic_Mesh1", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TempStatic_Mesh2", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_TempStatic_Mesh3", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_SkyBox(const _tchar* pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_SkyBox", SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Shader(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Default",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Default.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Texture",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Texture.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Terrain",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Terrain.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Terrain_Texture",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Terrain_Texture.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Normal",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Normal.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Mesh",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Mesh.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_SkyBox",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_SkyBox.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Toon_0",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Toon_0.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;


	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Toon_1",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Toon_1.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Hatching",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Hatching.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Sumuk",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Sumuk.hlsl", "VS_Main", "PS_Main", 0))))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Buffer(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_TriCol",
		CBuffer_TriCol::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_CubeCol",
		CBuffer_CubeCol::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_RectCol",
		CBuffer_RectCol::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_RectTex",
		CBuffer_RectTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Terrain",
		CBuffer_Terrain::Create(m_pGraphic_Device, 50, 50, 1.f))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Terrain_Texture",
		CBuffer_Terrain_Texture::Create(m_pGraphic_Device, 50, 50, 1.f))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Terrain_Height",
		CBuffer_Terrain_Height::Create(m_pGraphic_Device, L"../Resource/Height.bmp"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Cube_Tex",
		CBuffer_CubeTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Texture(CManagement* pManagement)
{
	//dds Texture
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Bricks",
		CTexture::Create(m_pGraphic_Device, L"../Resource/bricks%d.dds", 3, TEXTURE_TYPE_DDS))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_ELSE",
		CTexture::Create(m_pGraphic_Device, L"../Resource/SkyBox%d.dds", 0, TEXTURE_TYPE_DDS))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Hatching123",
		CTexture::Create(m_pGraphic_Device, L"../Resource/Hatching/Hatching%d.dds", 2, TEXTURE_TYPE_DDS))))
		return E_FAIL;

	//jpg Texture
	//if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_ELSE",
	//	CTexture::Create(m_pGraphic_Device, L"../Resource/else%d.jpg", 0, TEXTURE_TYPE_ELSE))))
	//	return E_FAIL;

	//png Texture
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Test",
		CTexture::Create(m_pGraphic_Device, L"../Resource/Test%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Mesh_Test",
		CTexture::Create(m_pGraphic_Device, L"../Resource/FBX/TestTexture/Kachujin_diffuse%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Mesh_Test1",
		CTexture::Create(m_pGraphic_Device, L"../Resource/FBX/TestTexture/Mutant_diffuse%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Mesh_Test2",
		CTexture::Create(m_pGraphic_Device, L"../Resource/FBX/TestTexture/bear_diffuse%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Mesh_Test3",
		CTexture::Create(m_pGraphic_Device, L"../Resource/FBX/TestTexture/MAW_diffuse%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;
	// Gray Scale










	//// MeshTest
	//if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_MeshTest",
	//	CTexture::Create(m_pGraphic_Device, L"../Resource/Rock01.fbm/rock_01_diffuse%d.jpg", 0, TEXTURE_TYPE_ELSE))))
	//	return E_FAIL;

	//// TGA 
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Grass",
		CTexture::Create(m_pGraphic_Device, L"../Resource/Grass_%d.tga", 0, TEXTURE_TGA))))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Static_Mesh(CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Static_Mesh_Temp",
	//	CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/Rock01.FBX"))))
	//	return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/FBX/Character/Idle.fbx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp1",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/FBX/Monster/Monster1/Idle.fbx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp2",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/FBX/Monster/Monster2/Idle.fbx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp3",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/FBX/Monster/Monster3/Idle.fbx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Test",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/Test0.fbx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Test1",
		CStatic_Mesh::Create(m_pGraphic_Device, "../Resource/Test1.fbx"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Dynamic_Mesh(CManagement* pManagement)
{
	//if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp",
	//	CDynamic_Mesh::Create(m_pGraphic_Device,"../Resource/FBX/Monster/Monster3/Idle.fbx"))))
	//	return E_FAIL;
	return S_OK;
}

CScene_Logo* CScene_Logo::Create(ID3D12Device* pGraphic_Device)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphic_Device);

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

	pManagement->Clear_Layers(SCENE_LOGO);

	Safe_Release(pManagement);

	CScene::Free();
}
