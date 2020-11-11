#include "framework.h"
#include "Scene_Logo.h"
#include "Management.h"
#include "Debug_Camera.h"


#include "Cube.h"
#include "Rect.h"
#include "TextureRect.h"
#include "Terrain.h"
#include "Texture_Terrain.h"
#include "TempMesh.h"

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
	//if(FAILED(Ready_Layer_Cube(L"Layer_Cube")))
	//	return E_FAIL;
	//if(FAILED(Ready_Layer_Rect(L"Layer_Rect")))
	//	return E_FAIL;
	if (FAILED(Ready_Layer_Temp_Mesh(L"Layer_TempMesh")))
		return E_FAIL;
	//if (FAILED(Ready_Layer_TextureRect(L"Layer_TextureRect")))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain")))
	//	return E_FAIL;
	//if (FAILED(Ready_Layer_Terrain_Texture(L"Layer_Terrain_Texture")))
	//	return E_FAIL;
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
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_TempMesh", CTempMesh::Create(m_pGraphic_Device))))
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
	if (FAILED(Add_Prototype_Component_Dynamic_Mesh(pManagement)))
		return E_FAIL;
	Safe_Release(pManagement);
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
	tProjDesc.fFar = 500.f;

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
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Mesh",
		CShader::Create(m_pGraphic_Device, L"../ShaderFiles/Shader_Mesh.hlsl", "VS_Main", "PS_Main", 0))))
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
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Texture(CManagement* pManagement)
{
	//dds Texture
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Bricks",
		CTexture::Create(m_pGraphic_Device, L"../Resource/bricks%d.dds", 3, TEXTURE_TYPE_DDS))))
		return E_FAIL;
	//jpg Texture
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_ELSE",
		CTexture::Create(m_pGraphic_Device, L"../Resource/else%d.jpg", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;
	//png Texture
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Test",
		CTexture::Create(m_pGraphic_Device, L"../Resource/Test%d.png", 0, TEXTURE_TYPE_ELSE))))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CScene_Logo::Add_Prototype_Component_Dynamic_Mesh(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Dynamic_Mesh_Temp",
		CDynamic_Mesh::Create(m_pGraphic_Device,"../Resource/FBX/Angrybot.FBX"))))
		return E_FAIL;
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
