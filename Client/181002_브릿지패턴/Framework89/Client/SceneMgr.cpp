#include "stdafx.h"
#include "SceneMgr.h"
#include "Stage.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	: m_pScene(nullptr), m_eCurScene(END), m_ePreScene(END)
{
}


CSceneMgr::~CSceneMgr()
{
	Release();
}

HRESULT CSceneMgr::SceneChange(SCENEID eCurScene)
{
	if (m_ePreScene != eCurScene)
	{
		SafeDelete(m_pScene);

		switch (eCurScene)
		{
		case LOGO:
			break;
		case STAGE:
			m_pScene = new CStage;
			break;
		}

		if (FAILED(m_pScene->Initialize()))
		{
			ERR_MSG(L"Scene Create Failed");
			return E_FAIL;
		}

		m_eCurScene = eCurScene;
		m_ePreScene = eCurScene;
	}

	return S_OK;
}

void CSceneMgr::Update()
{
	m_pScene->Update();
}

void CSceneMgr::LateUpdate()
{
	m_pScene->LateUpdate();
}

void CSceneMgr::Render()
{
	m_pScene->Render();
}

void CSceneMgr::Release()
{
	SafeDelete(m_pScene);
}
