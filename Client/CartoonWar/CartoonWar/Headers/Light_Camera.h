#pragma once
#include "Camera.h"
class CLight_Camera :
	public CCamera
{
private:
	explicit CLight_Camera();
	explicit CLight_Camera(const CLight_Camera& rhs);
	virtual ~CLight_Camera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float& fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void				Render_GameObject();
public:
	static CLight_Camera* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
protected:
	virtual void				Free();
private:
	POINT						m_ptMouse;
};

