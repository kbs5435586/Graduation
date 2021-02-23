#pragma once

class CScene;
class CSceneMgr
{
public:
	enum SCENEID { LOGO, MENU, STAGE, END };

	DECLARE_SINGLETON(CSceneMgr)

private:
	CSceneMgr();
	~CSceneMgr();

public:
	HRESULT SceneChange(SCENEID eCurScene);
	void Update();
	void LateUpdate();
	void Render();
	void Release();

private:
	SCENEID		m_eCurScene;
	SCENEID		m_ePreScene;

	CScene*		m_pScene;
};

