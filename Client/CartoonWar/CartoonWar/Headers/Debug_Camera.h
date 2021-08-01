#pragma once
#include "Camera.h"
class CNavigation;
class CObserver;
class CDebug_Camera :
    public CCamera
{
private:
	explicit CDebug_Camera();
	explicit CDebug_Camera(const CDebug_Camera& rhs);
	virtual ~CDebug_Camera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float& fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void				Render_GameObject();

	HRESULT									Ready_Component();
public:
	static CDebug_Camera*		Create();
	virtual CGameObject*		Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
protected:
	virtual void				Free();
private:
	POINT						m_ptMouse;
	CNavigation*				m_pNaviCom = nullptr;
	//CObserver*					m_pObserverCom = nullptr;
private:
	_bool						m_IsTrue = false;
	_bool						m_IsFix = false;
	_bool						m_IsChangeIn = false;
	_bool						m_Active = false;
};

