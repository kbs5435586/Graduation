#include "stdafx.h"
#include "Tool_Scene.h"
#include "Management.h"
#include "Light_Mgr.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "EffectBuffer.h"
//Cliff
#include "Cliff0.h"
#include "Cliff1.h"
#include "Cliff2.h"
#include "Cliff3.h"
#include "Cliff4.h"
//Fence
#include "Fence0.h"
#include "Fence1.h"
#include "Fence2.h"
#include "Fence3.h"
#include "Fence4.h"
#include "Fence5.h"
#include "Fence6.h"
#include "Fence7.h"
//Flower
#include "Flower0.h"
#include "Flower1.h"
#include "Flower2.h"
#include "Flower3.h"
#include "Flower4.h"
#include "Flower5.h"
#include "Flower6.h"
#include "Flower7.h"
#include "Flower8.h"
#include "Flower9.h"
#include "Flower10.h"
//Grass
#include "Grass0.h"
#include "Grass1.h"
#include "Grass2.h"
#include "Grass3.h"
#include "Grass4.h"
#include "Grass5.h"
#include "Grass6.h"
#include "Grass7.h"
#include "Grass8.h"
#include "Grass9.h"
#include "Grass10.h"
#include "Grass11.h"
#include "Grass12.h"
#include "Grass13.h"
#include "Grass14.h"
#include "Grass15.h"
#include "Grass16.h"
#include "Grass17.h"
#include "Grass18.h"
#include "Grass19.h"
//Hill
#include "Hill0.h"
#include "Hill1.h"
#include "Hill2.h"
//House
#include "House0.h"
//Rock
#include "Rock0.h"
#include "Rock1.h"
#include "Rock2.h"
#include "Rock3.h"
#include "Rock4.h"
#include "Rock5.h"
#include "Rock6.h"
//Tree
#include "Tree0.h"
#include "Tree1.h"
#include "Tree2.h"
#include "Tree3.h"
#include "Tree4.h"
#include "Tree5.h"
#include "Tree6.h"
#include "Tree7.h"
#include "Tree8.h"
//Wall
#include "Wall0.h"
#include "Wall1.h"
#include "Wall2.h"
#include "Wall3.h"
#include "Wall4.h"
#include "Wall5.h"
#include "Wall6.h"
#include "Wall7.h"
#include "Wall8.h"
#include "Wall9.h"
#include "Wall10.h"
//SkHouse
#include "SkHouse0.h"
#include "SkHouse1.h"
#include "SkHouse2.h"
//Dynamic_Mesh
#include "Player.h"
#include "Beatle.h"
#include "Bee.h"
#include "Mantis.h"
#include "Monkey.h"
#include "NPC.h"
#include "Tower.h"
#include "Vergos.h"
#include "UD_Worker.h"
#include "UD_LightCarvarly.h"
#include "UD_HeavyInfantry.h"
//UI
#include "HP.h"
#include "MP.h"
#include "Back_Hp.h"
#include "Back_Mp.h"


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
	// Cliff
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cliff0", CCliff0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cliff1", CCliff1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cliff2", CCliff2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cliff3", CCliff3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Cliff4", CCliff4::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Fence
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence0", CFence0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence1", CFence1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence2", CFence2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence3", CFence3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence4", CFence4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence5", CFence5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence6", CFence6::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Fence7", CFence7::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Flower
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower0", CFlower0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower1", CFlower1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower2", CFlower2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower3", CFlower3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower4", CFlower4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower5", CFlower5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower6", CFlower6::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower7", CFlower7::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower8", CFlower8::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower9", CFlower9::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Flower10", CFlower10::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Grass
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass0", CGrass0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass1", CGrass1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass2", CGrass2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass3", CGrass3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass4", CGrass4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass5", CGrass5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass6", CGrass6::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass7", CGrass7::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass8", CGrass8::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass9", CGrass9::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass10", CGrass10::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass11", CGrass11::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass12", CGrass12::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass13", CGrass13::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass14", CGrass14::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass15", CGrass15::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass16", CGrass16::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass17", CGrass17::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass18", CGrass18::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Grass19", CGrass19::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Hill
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Hill0", CHill0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Hill1", CHill1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Hill2", CHill2::Create(m_pGraphic_Device))))
		return E_FAIL;
	//House
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_House0", CHouse0::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Rock
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock0", CRock0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock1", CRock1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock2", CRock2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock3", CRock3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock4", CRock4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock5", CRock5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Rock6", CRock6::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Tree
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree0", CTree0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree1", CTree1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree2", CTree2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree3", CTree3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree4", CTree4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree5", CTree5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree6", CTree6::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree7",CTree7::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tree8", CTree8::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Wall
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall0", CWall0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall1", CWall1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall2", CWall2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall3", CWall3::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall4", CWall4::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall5", CWall5::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall6", CWall6::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall7", CWall7::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall8", CWall8::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall9", CWall9::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Wall10", CWall10::Create(m_pGraphic_Device))))
		return E_FAIL;
	//SkHouse
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkHouse0", CSkHouse0::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkHouse1", CSkHouse1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_SkHouse2", CSkHouse2::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Dynamic_Mesh
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Popori", CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Beatle", CBeatle::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Bee", CBee::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Mantis", CMantis::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Monkey", CMonkey::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_NPC", CNPC::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Tower", CTower::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_Vergos", CVergos::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UD_Worker", CUD_Worker::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UD_LightCarvarly", CUD_LightCarvarly::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UD_HeavyInfantry", CUD_HeavyInfantry::Create(m_pGraphic_Device))))
		return E_FAIL;

	//UI
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_HP", CHP::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_MP", CMP::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_HP_Back", CBack_Hp::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_GameObject(L"GameObject_UI_MP_Back", CBack_Mp::Create(m_pGraphic_Device))))
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

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Default",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Terrain",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Mesh",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Mesh.fx"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Terrain",
		CBuffer_Terrain::Create(m_pGraphic_Device, 100, 100, 1.f))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_Texture",
		CBuffer_RcTex::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Terrain",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Client/Bin/Texture/Terrain/Grass_%d.tga", 2))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Default",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Temp/Explosion%d.png", 1))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Test",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Temp/Test%d.png", 1))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Sky",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_CUBE, L"../Client/Bin/Texture/SkyBox/Burger%d.dds", 4))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Arcane",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Effect/Texture/Arcane/Arcane_%d.png", 41))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Flame",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Effect/Texture/Flame_Particle/%d.png", 60))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Picking", 
		CPicking::Create(m_pGraphic_Device, g_hWnd))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Font",
		CMyFont::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_NaviMesh", 
		CNavi::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Shader_Sky",
		CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Sky.fx"))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Buffer_CubeTex",
		CBuffer_CubeTex::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_SkyBox",
		CStatic_Mesh::Create(m_pGraphic_Device, L"../Client/Bin/Meshes/StaticMesh/SkyBox/SkyBox/", L"SkyBox.X"))))
		return E_FAIL;

	//UI
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_HP",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Hp/Hp%d.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_HP_Back",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Hp_Back/Hp_Back%d.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_MP",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Mp/Mp%d.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_MP_Back",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Mp_Back/Mp_Back%d.png", 1))))
		return E_FAIL; 
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Monster_HP",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Monster_Hp/Monster_Hp%d.tga", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Texture_Monster_HP_Back",
		CTexture::Create(m_pGraphic_Device, TEXTURE_TYPE_GENERAL, L"../Resource/Texture/UI/Bar/Monster_Hp_Back/Monster_Hp_Back%d.tga", 1))))
		return E_FAIL;
	//Cliff
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Cliff0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Cliff/Cliff0/", L"Cliff0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Cliff1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Cliff/Cliff1/", L"Cliff1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Cliff2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Cliff/Cliff2/", L"Cliff2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Cliff3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Cliff/Cliff3/", L"Cliff3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Cliff4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Cliff/Cliff4/", L"Cliff4.X"))))
			return E_FAIL;
	}
	//Fence
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence0/", L"Fence0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence1/", L"Fence1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence2/", L"Fence2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence3/", L"Fence3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence4/", L"Fence4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence5/", L"Fence5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence6/", L"Fence6.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Fence7",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Fence/Fence7/", L"Fence7.X"))))
			return E_FAIL;
	}
	//Flower
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower0/", L"Flower0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower1/", L"Flower1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower2/", L"Flower2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower3/", L"Flower3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower4/", L"Flower4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower5/", L"Flower5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower6/", L"Flower6.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower7",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower7/", L"Flower7.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower8",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower8/", L"Flower8.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower9",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower9/", L"Flower9.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Flower10",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Flower/Flower10/", L"Flower10.X"))))
			return E_FAIL;
	}
	//Grass
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass0/", L"Grass0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass1/", L"Grass1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass2/", L"Grass2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass3/", L"Grass3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass4/", L"Grass4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass5/", L"Grass5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass6/", L"Grass6.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass7",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass7/", L"Grass7.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass8",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass8/", L"Grass8.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass9",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass9/", L"Grass9.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass10",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass10/", L"Grass10.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass11",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass11/", L"Grass11.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass12",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass12/", L"Grass12.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass13",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass13/", L"Grass13.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass14",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass14/", L"Grass14.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass15",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass15/", L"Grass15.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass16",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass16/", L"Grass16.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass17",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass17/", L"Grass17.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass18",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass18/", L"Grass18.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Grass19",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Grass/Grass19/", L"Grass19.X"))))
			return E_FAIL;
	}
	//Hill
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Hill0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Hill/Hill0/", L"Hill0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Hill1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Hill/Hill1/", L"Hill1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Hill2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Hill/Hill2/", L"Hill2.X"))))
			return E_FAIL;
	}
	//House
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_House0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/House/House0/", L"House0.X"))))
			return E_FAIL;
	}
	//Rock
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock0/", L"Rock0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock1/", L"Rock1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock2/", L"Rock2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock3/", L"Rock3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock4/", L"Rock4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock5/", L"Rock5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Rock6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Rock/Rock6/", L"Rock6.X"))))
			return E_FAIL;
	}
	//Tree
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree0/", L"Tree0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree1/", L"Tree1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree2/", L"Tree2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree3/", L"Tree3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree4/", L"Tree4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree5/", L"Tree5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree6/", L"Tree6.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree7",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree7/", L"Tree7.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Tree8",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Tree/Tree8/", L"Tree8.X"))))
			return E_FAIL;
	}
	//Wall
	{

		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall0/", L"Wall0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall1/", L"Wall1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall2/", L"Wall2.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall3",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall3/", L"Wall3.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall4",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall4/", L"Wall4.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall5",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall5/", L"Wall5.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall6",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall6/", L"Wall6.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall7",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall7/", L"Wall7.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall8",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall8/", L"Wall8.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall9",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall9/", L"Wall9.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_Wall10",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/Wall/Wall10/", L"Wall10.X"))))
			return E_FAIL;
	}
	//SkHouse
	{
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_SkHouse0",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/SkHouse/", L"SkHouse0.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_SkHouse1",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/SkHouse/", L"SkHouse1.X"))))
			return E_FAIL;
		if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_StaticMesh_SkHouse2",
			CStatic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/StaticMesh/SkHouse/", L"SkHouse2.X"))))
			return E_FAIL;

	}
	//Dynamic_Meshes
	{
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Popori",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/PoPori/", L"PoPori.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Beatle",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Beatle/", L"Beatle.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Bee",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Bee/", L"Bee.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Mantis",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Mantis/", L"Mantis.X"))))
		return E_FAIL;

	if(FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Monkey",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Monkey/", L"Monkey.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_NPC",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/NPC/", L"NPC.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Tower",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Tower/", L"Tower.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_Vergos",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/Vergos/", L"Vergos.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_UD_Worker",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/UD/", L"UD_Worker.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_UD_LightCarvarly",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/UD/", L"UD_LightCavalry.X"))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, L"Component_Mesh_UD_HeavyInfantry",
		CDynamic_Mesh::Create(m_pGraphic_Device, L"../Resource/Meshes/DynamicMesh/UD/", L"UD_HeavyInfantry.X"))))
		return E_FAIL;
	}

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
	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Popori", SCENE_LOGO, pLayerTag)))
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
