#pragma once
#include "Base.h"
class CGameObject_Mgr;
class CScene :
	public CBase
{
public:
	CScene(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene() = default;
public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(const _float& fTimeDelta);
	virtual _int LastUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene();
public:
	virtual void Free();
protected:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
private:
	CGameObject_Mgr*		m_pGameObject_Mgr = nullptr;
};

