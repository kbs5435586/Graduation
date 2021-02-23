#include "stdafx.h"
#include "Maingame.h"


CMaingame::CMaingame()
	: m_pDevice(GET_INSTANCE(CDevice)), m_pSceneMgr(GET_INSTANCE(CSceneMgr))
{
	
}


CMaingame::~CMaingame()
{
	Release();
}

HRESULT CMaingame::Initialize()
{
	if (FAILED(m_pDevice->InitDevice()))
		return E_FAIL;

	if (FAILED(m_pSceneMgr->SceneChange(CSceneMgr::STAGE)))
	{
		ERR_MSG(L"Scene change Failed!!");
		return E_FAIL;
	}

	GET_INSTANCE(CTimeMgr)->InitTime();

	return S_OK;
}

void CMaingame::Update()
{
	GET_INSTANCE(CTimeMgr)->UpdateTime();
	GET_INSTANCE(CKeyMgr)->Update();
	m_pSceneMgr->Update();
}

void CMaingame::LateUpdate()
{
	m_pSceneMgr->LateUpdate();
}

void CMaingame::Render()
{
	m_pDevice->Render_Begin();

	m_pSceneMgr->Render();
	GET_INSTANCE(CFrameMgr)->RenderFrame();	

	m_pDevice->Render_End();
}

void CMaingame::Release()
{	
	GET_INSTANCE(CAstarMgr)->DestroyInstance();
	GET_INSTANCE(CKeyMgr)->DestroyInstance();
	GET_INSTANCE(CTimeMgr)->DestroyInstance();
	GET_INSTANCE(CSceneMgr)->DestroyInstance();
	GET_INSTANCE(CObjMgr)->DestroyInstance();
	GET_INSTANCE(CTextureMgr)->DestroyInstance();
	GET_INSTANCE(CDevice)->DestroyInstance();
}
