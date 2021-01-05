#pragma once
#include "Component.h"
class CTransform :
	public CComponent
{
private:
	explicit			CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit			CTransform(const CTransform& rhs);
	virtual				~CTransform() = default;
public: // Getter
	const _vec3*		Get_StateInfo(STATE eState)const;
	_vec3				Get_Scale();
	_matrix				Get_Matrix_Inverse() const;
	_matrix				Get_Matrix()const { return m_matWorld; }
	const _vec3			Get_Rotate() { return m_vRotate; }
	const _float		Get_Add_PosY() { return m_fAdd_PosY; }
public: // Setter
	void				Set_StateInfo(STATE eState, const _vec3* pInfo);
	void				Set_PositionY(const _float& fY) {m_matWorld.m[3][1] = fY;}
	void				Set_Matrix(_matrix mat) { m_matWorld = mat; }
	void				Set_Rotate(_vec3 vRot);
	void				Set_Add_PosY(_float fY) { m_fAdd_PosY = fY; }
public:
	HRESULT				Ready_Transform();
	HRESULT				SetUp_OnGraphicDev();
	HRESULT				SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
public:
	void				SetUp_Speed(const _float& fMovePerSec, const _float& fRotationPerSec);
	void				Go_Straight(const _float& fTimeDelta);
	void				Go_Left(const _float& fTimeDelta);
	void				Go_Right(const _float& fTimeDelta);
	void				BackWard(const _float& fTimeDelta);
	void				SetUp_RotationX(const _float& fRadian);
	void				SetUp_RotationY(const _float& fRadian);
	void				SetUp_RotationZ(const _float& fRadian);
	void				Rotation_X(const _float& fTimeDelta);
	void				Rotation_Y(const _float& fTimeDelta);
	void				Rotation_Z(const _float& fTimeDelta);
	void				Rotation_Axis(const _float& fTimeDelta, const _vec3* pAxis);
	void				Scaling(const _float& fX, const _float& fY, const _float& fZ);
	void				Go_ToTarget(const _vec3* pTargetPos, const _float& fTimeDelta);
private:
	_matrix				m_matWorld; // °´Ã¼ÀÇ »óÅÂ.
	_float				m_fSpeed_Move;
	_float				m_fSpeed_Rotation;
	_vec3				m_vRotate = _vec3(0.f,0.f,0.f);
	_float				m_fAdd_PosY = 0.f;
public:
	static CTransform*	Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void		Free();
};

