#pragma once
#include "Camera.h"
class CCamera_Debug :
	public CCamera
{
private:
	explicit CCamera_Debug(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Debug(const CCamera_Debug& rhs);
	virtual ~CCamera_Debug() = default;
public:
	virtual HRESULT Ready_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject();
public:
	static CCamera_Debug* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
protected:
	virtual void Free();
private:
	POINT			m_ptMouse;
};

