#include "stdafx.h"
#include "Stage.h"
#include "Terrain.h"
#include "Player.h"

CStage::CStage()
{
}


CStage::~CStage()
{
	Release();
}

HRESULT CStage::Initialize()
{
	if (FAILED(GET_INSTANCE(CTextureMgr)->ReadImgPath(L"../Data/ImgPathInfo.txt")))
		return E_FAIL;

	CObj* pObj = CAbstractFactory<CTerrain>::CreateObj();
	NULL_CHECK_VAL(pObj, E_FAIL);
	GET_INSTANCE(CObjMgr)->AddObject(pObj, CObjMgr::TERRAIN);

	pObj = CAbstractFactory<CPlayer>::CreateObj();
	NULL_CHECK_VAL(pObj, E_FAIL);
	GET_INSTANCE(CObjMgr)->AddObject(pObj, CObjMgr::PLAYER);

	return S_OK;
}

void CStage::Update()
{
	GET_INSTANCE(CObjMgr)->Update();
}

void CStage::LateUpdate()
{
	GET_INSTANCE(CObjMgr)->LateUpdate();
}

void CStage::Render()
{
	GET_INSTANCE(CObjMgr)->Render();
}

void CStage::Release()
{
}
