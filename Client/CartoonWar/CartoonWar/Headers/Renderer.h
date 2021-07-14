#pragma once
#include "Component.h"
class CGameObject;
class CManagement;
class CRenderer :
	public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONEALPHA_PRO, RENDER_NONEALPHA, RENDER_LIGHT, RENDER_ALPHA,RENDER_UI, RENDER_UI_BACK, RENDER_BLEND, RENDER_SHADOW, RENDER_POST, RENDER_BLUR, RENDER_REF, RENDER_END };
private:
	explicit						CRenderer();
	virtual							~CRenderer() = default;
public:
	HRESULT							Ready_Renderer();
	HRESULT							Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject);
	HRESULT							Render_RenderGroup();
private:
	void							CopySwapToPosteffect();
	void							CopySwapToBlur();
	void							CopySwapToReflection();
private:
	void							Render_Priority();
	void							Render_NoneAlpha();
	void							Render_NoneAlpha_PRO();
	void							Render_Alpha();
	void							Render_UI();
	void							Render_UI_Back();
	void							Render_Blend();
	void							Render_Post_Effect();
	void							Render_Blur();
	void							Render_Reflection();
private:
	void							Render_Deffered(CManagement* pManagement);
	void							Render_Shadow(CManagement* pManagement);
	void							Render_Light(CManagement* pManagement);
public:
	static CRenderer*				Create();
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();
private:
	list<CGameObject*>				m_RenderList[RENDER_END];
	typedef list<CGameObject*>		OBJECTLIST;

};

