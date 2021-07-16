#pragma once
#include "Camera.h"
class CReflection_Camera :
    public CCamera
{
private:
	explicit CReflection_Camera();
	explicit CReflection_Camera(const CReflection_Camera& rhs);
	virtual ~CReflection_Camera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float & fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void				Render_GameObject();
public:
	static CReflection_Camera* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
protected:
	virtual void				Free();
private:
	POINT						m_ptMouse;

};

