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
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, _int iTemp);
	void						Invalidate_ViewProjMatrix(_int iTemp);
	HRESULT						SetUp_ViewProjMatrices(_int iTemp);
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, _bool IsShadow);
	HRESULT						SetUp_ViewProjMatrices(_bool IsShadow);
	void						Invalidate_ViewProjMatrix(_bool IsShadow);
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, int n);
	HRESULT						SetUp_ViewProjMatrices(int n);
	void						Invalidate_ViewProjMatrix(int n);
	HRESULT						Once_SetUp_ViewProjMatrices(_bool IsShadow, _vec3 vPos);
private:
	HRESULT						SetUp_ViewProjMatrices();
public:
	virtual CGameObject*		Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) PURE;
protected:
	virtual void				Free();
public:
	CTransform*					GetTransform(){return m_pTransform;}
protected:
	CInput*						m_pInput_Device = nullptr;
protected:
	CTransform*					m_pTransform = nullptr;

	_matrix						m_matView;
	CAMERADESC					m_tCameraDesc;
protected:
	_matrix						m_matProj;
	PROJDESC					m_tProjDesc;
protected:
	_matrix						m_matShadowView;
	CAMERADESC					m_tShadowCameraDesc;
protected:
	_matrix						m_matShadowProj;
	PROJDESC					m_tShadowProjDesc;
protected:
	_matrix						m_matInventoryView;
	CAMERADESC					m_tInventoryCameraDesc;
protected:
	_matrix						m_matInventoryProj;
	PROJDESC					m_tInventoryProjDesc;
protected:
	_matrix						m_matReflectionView;
	CAMERADESC					m_tReflectionCameraDesc;
protected:
	_matrix						m_matReflectionProj;
	PROJDESC					m_tReflectionProjDesc;

protected:
	_vec3						m_vRight, m_vUp, m_vLook, m_vPos; 

};

