#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Management.h"
#include "Camera_Debug.h"
#include "Test.h"
#include "Terrain.h"
#include "Monster.h"
#include "SkyBox.h"
#include "Stone.h"

#include "Light_Mgr.h"

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround(L"Layer_Terrain")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkyBox(L"Layer_SkyBox")))
		return E_FAIL;
	if (FAILED(Ready_Layer_Stone(L"Layer_Stone")))
		return E_FAIL;
	if (FAILED(Ready_LightInfo()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Stage::Update_Scene(const _float & fTimeDelta)
{
	return CScene::Update_Scene(fTimeDelta);
}

_int CScene_Stage::LastUpdate_Scene(const _float & fTimeDelta)
{
	return CScene::LastUpdate_Scene(fTimeDelta);
}

void CScene_Stage::Render_Scene()
{
}

HRESULT CScene_Stage::Ready_Prototype_GameObject()
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Monster", CMonster::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkyBox", CSkyBox::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Stone", CStone::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Prototype_Component()
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();


	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Buffer_Terrain", CBuffer_Terrain::Create(m_pGraphic_Device, L"../Bin/Texture/Terrain/Height.bmp",1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Buffer_CubeTex", CBuffer_CubeTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Texture_Terrain", CTexture::Create( m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Bin/Texture/Terrain/Grass_%d.tga",2))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Texture_Monster", CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Bin/Texture/Monster/Monster0.png"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Picking", CPicking::Create(m_pGraphic_Device, g_hWnd))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Texture_SkyBox", CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_CUBE, L"../Bin/Texture/SkyBox/burger%d.dds",4))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Shader_Terrain", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Shader_Mesh", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Mesh.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_STAGE, L"Component_Mesh_Stone", CStatic_Mesh::Create(m_pGraphic_Device, L"../Bin/Meshes/StaticMesh/TombStone/", L"TombStone.X"))))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}



HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Terrain", SCENE_STAGE, pLayerTag)))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return E_FAIL;

	pManagement->AddRef();
	CCamera_Debug*		pCameraObject = nullptr;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Camera_Debug", SCENE_STAGE, pLayerTag, (CGameObject**)&pCameraObject)))
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

HRESULT CScene_Stage::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	for (int i = 0; i < 20; ++i)
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Monster", SCENE_STAGE, pLayerTag)))
			return E_FAIL;
	}


	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_SkyBox", SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Stone(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Stone", SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_LightInfo()
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

	LightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	if (FAILED(pLight_Manager->Add_LightInfo(m_pGraphic_Device, LightInfo)))
		return E_FAIL;


	Safe_Release(pLight_Manager);

	return S_OK;
}

CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Stage*	pInstance = new CScene_Stage(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);
	return pInstance;
}

void CScene_Stage::Free()
{
	CManagement*		pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return;

	pManagement->AddRef();

	pManagement->Clear_Layers(SCENE_STAGE);

	Safe_Release(pManagement);

	CScene::Free();
}
