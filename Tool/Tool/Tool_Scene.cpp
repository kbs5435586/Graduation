#include "stdafx.h"
#include "Tool_Scene.h"
#include "Management.h"
#include "Light_Mgr.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "EffectBuffer.h"

#include "Tree.h"
 

CTool_Scene::CTool_Scene(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CScene(pGraphic_Device)
{
}
HRESULT CTool_Scene::Ready_Scene()
{
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if (FAILED(Ready_LightInfo()))
		return E_FAIL;
	if (FAILED(Ready_Camera_Layer(L"Layer_Camera")))
		return E_FAIL;
	if (FAILED(Ready_Terrain_Layer(L"Layer_Terrain")))
		return E_FAIL;
	if (FAILED(Ready_SkyBox_Layer(L"Layer_SkyBox")))
		return E_FAIL;
	if (FAILED(Ready_Player_Layer(L"Layer_Player")))
		return E_FAIL;

	return S_OK;
}

_int CTool_Scene::Update_Scene(const _float & fTimeDelta)
{
	return CScene::Update_Scene(fTimeDelta);
}

_int CTool_Scene::LastUpdate_Scene(const _float & fTimeDelta)
{
	return CScene::LastUpdate_Scene(fTimeDelta); return _int();
}

void CTool_Scene::Render_Scene()
{
}

HRESULT CTool_Scene::Ready_Component_Texture(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Terrain",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/Grass/Grass_%d.tga"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_SkyBox",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_CUBE, L"../Resource/Texture/SkyBox/SkyBox%d.dds",5))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTool_Scene::Ready_Component_Mesh(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree",
		CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Mesh/Tree/", L"tree_1.x"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTool_Scene::Ready_Component_Shader(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Default",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Terrain",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Mesh",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Mesh.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Sky",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Sky.fx"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTool_Scene::Ready_Component_Buffer(CManagement* pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Terrain",
		CBuffer_Terrain::Create(m_pGraphic_Device, 100, 100, 1.f))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Texture",
		CBuffer_RcTex::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_CubeTex",
		CBuffer_CubeTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTool_Scene::Ready_Component_Function(CManagement * pManagement)
{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Picking",
		CPicking::Create(m_pGraphic_Device, g_hWnd))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Font",
		CMyFont::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_NaviMesh",
		CNavi::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTool_Scene::Ready_Prototype_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_EffectBuffer", CEffectBuffer::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree", CTree::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CTool_Scene::Ready_Prototype_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	if (FAILED(Ready_Component_Texture(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Component_Function(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Component_Buffer(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Component_Shader(pManagement)))
		return E_FAIL;
	if (FAILED(Ready_Component_Mesh(pManagement)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
HRESULT CTool_Scene::Ready_LightInfo()
{
	CLight_Mgr*		pLight_Manager = CLight_Mgr::GetInstance();
	if (nullptr == pLight_Manager)
		return E_FAIL;

	pLight_Manager->AddRef();

	D3DLIGHT9			LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	LightInfo.Direction = _vec3(1.f, 1.f, 1.f);

	if (FAILED(pLight_Manager->Add_LightInfo(m_pGraphic_Device, LightInfo)))
		return E_FAIL;


	Safe_Release(pLight_Manager);

	return S_OK;
}
HRESULT CTool_Scene::Ready_Camera_Layer(const _tchar * pLayerTag)
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();
	CCamera_Debug*		pCameraObject = nullptr;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Debug", SCENE_LOGO, pLayerTag, nullptr, (CGameObject**)&pCameraObject)))
		return E_FAIL;

	CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CAMERADESC));
	CameraDesc.vEye = _vec3(0.f, 20.f, -10.f);
	CameraDesc.vAt = _vec3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);

	PROJDESC		ProjDesc;
	ZeroMemory(&ProjDesc, sizeof(PROJDESC));
	ProjDesc.fFovY = D3DXToRadian(60.0f);
	ProjDesc.fAspect = _float(g_iBackCX) / g_iBackCY;
	ProjDesc.fNear = 0.2f;
	ProjDesc.fFar = 500.0f;

	if (FAILED(pCameraObject->SetUp_CameraProjDesc(CameraDesc, ProjDesc)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
HRESULT CTool_Scene::Ready_Terrain_Layer(const _tchar * pLayerTag)
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
HRESULT CTool_Scene::Ready_SkyBox_Layer(const _tchar * pLayerTag)
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
HRESULT CTool_Scene::Ready_Player_Layer(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Tree", SCENE_LOGO, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}
CTool_Scene * CTool_Scene::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTool_Scene* pInstance = new CTool_Scene(pGraphic_Device);
	if (FAILED(pInstance->Ready_Scene()))
		return nullptr;
	return pInstance;
}
void CTool_Scene::Free()
{
	CManagement*		pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;

	pManagement->AddRef();

	pManagement->Clear_Layers(SCENE_LOGO);

	Safe_Release(pManagement);
	CScene::Free();
}