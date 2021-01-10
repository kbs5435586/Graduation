#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Transform.h"
class CCamera :
    public CGameObject
{
protected:
	explicit CCamera( );
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float & fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void				Render_GameObject();
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC & CameraDesc, const PROJDESC & ProjDesc);
	void						Invalidate_ViewProjMatrix();
private:
	HRESULT						SetUp_ViewProjMatrices();
public:
	virtual CGameObject*		Clone_GameObject(void* pArg = nullptr) = 0;
protected:
	virtual void				Free();
protected:
	CInput*						m_pInput_Device = nullptr;
protected:
	//_matrix						m_matWorld;
	CTransform*					m_pTransform = nullptr;
	_matrix						m_matView;
	CAMERADESC					m_tCameraDesc;
protected:
	_matrix						m_matProj;
	PROJDESC					m_tProjDesc;

};

