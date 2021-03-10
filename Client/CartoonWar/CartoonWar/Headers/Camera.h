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
	_matrix						Calculate_RelfectMatrix(const _float& fHeight);
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC & CameraDesc, const PROJDESC & ProjDesc);
	void						Invalidate_ViewProjMatrix();
private:
	HRESULT						SetUp_ViewProjMatrices();
public:
	virtual CGameObject*		Clone_GameObject(void* pArg = nullptr, const _uint& iIdx = 0) = 0;
protected:
	virtual void				Free();
protected:
	CInput*						m_pInput_Device = nullptr;
protected:
	CTransform*					m_pTransform = nullptr;
	CTransform*					m_pTransform_Reflect = nullptr;
	_matrix						m_matView;
	CAMERADESC					m_tCameraDesc;
protected:
	_matrix						m_matProj;
	PROJDESC					m_tProjDesc;
protected:
	_matrix						m_matReflect;
protected:
	_vec3						m_vRight, m_vUp, m_vLook, m_vPos; 

};

