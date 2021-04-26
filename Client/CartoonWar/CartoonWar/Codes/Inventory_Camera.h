#pragma once
#include "Camera.h"
class CObserver;

class CInventory_Camera :
    public CCamera
{
private:
	explicit CInventory_Camera();
	explicit CInventory_Camera(const CInventory_Camera& rhs);
	virtual ~CInventory_Camera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float & fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void				Render_GameObject();
public:
	static CInventory_Camera* Create();
	virtual CGameObject* Clone_GameObject(void* pArg, const _uint& iIdx);

	HRESULT						Ready_Component();
protected:
	virtual void				Free();
private:
	POINT					m_ptMouse;
	//_vec3					m_vPos;
	CObserver* m_pObserverCom = nullptr;
};

