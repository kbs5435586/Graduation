#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Input.h"
class CCamera :
	public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;
public:
	virtual HRESULT Ready_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject();
public:
	HRESULT SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc);
	void Invalidate_ViewProjMatrix();
protected:
	CInput*	m_pInput_Device = nullptr;
protected:
	CTransform*		m_pTransform = nullptr;
	_matrix			m_matView;
	CAMERADESC		m_CameraDesc;
protected:
	_matrix			m_matProj;
	PROJDESC		m_ProjDesc;
private:
	HRESULT SetUp_ViewProjMatrices();
public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0;
protected:
	virtual void Free();
};

