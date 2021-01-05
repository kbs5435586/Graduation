#pragma once
#include "Component.h"

class CGameObject;
class CRenderer :
	public CComponent
{
private:
	CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer()=default;

public:
	HRESULT Ready_Renderer();
	HRESULT Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject);
	HRESULT Render_RenderGroup();
private:
	void Render_Priority();
	void Render_NoneAlpha();
	void Render_Alpha();
	void Render_UI();
public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device); 
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
private:
	vector<CGameObject*>			m_RenderList[RENDER_END];
	typedef vector<CGameObject*>	OBJECTLIST;
};

