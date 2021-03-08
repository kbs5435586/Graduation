#pragma once
#include "Component.h"
class CGameObject;
class CManagement;
class CRenderer :
    public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONEALPHA, RENDER_LIGHT ,RENDER_ALPHA, RENDER_UI, RENDER_UI_DEFFERED, RENDER_BLEND,RENDER_END };
private:
	explicit						CRenderer();
	virtual							~CRenderer() = default;
public:
	HRESULT							Ready_Renderer();
	HRESULT							Add_RenderGroup(RENDERGROUP eGroup, CGameObject * pGameObject);
	HRESULT							Render_RenderGroup();
private:
	void							Render_Priority();
	void							Render_NoneAlpha();
	void							Render_Alpha();
	void							Render_UI();
	void							Render_UI_DEFFERED();
	void							Render_Blend();
private:
	void							Render_Deffered(CManagement* pManagement);
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

