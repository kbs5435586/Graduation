#pragma once
#include "Component.h"
class CTransform :
    public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
private:
	CTransform();
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
public: // Getter
	_vec3*					Get_StateInfo(STATE eState);
	_vec3					Get_Scale();
	_matrix					Get_Matrix_Inverse() const;
	_matrix					Get_Matrix() { return m_matWorld; }
	const _float			Get_Add_PosY() { return m_fAdd_PosY; }
public:
	void					Set_Matrix(_matrix matWorld) { m_matWorld = matWorld; }
	void					Set_Matrix(_matrix matWorld, _bool IsMul);
public: // Setter
	void					Set_StateInfo(STATE eState, const _vec3* pInfo);
	void					Set_PositionY(const _float& fY) {	m_matWorld.m[3][1] = fY;}
	void					Set_Add_PosY(_float fY) { m_fAdd_PosY = fY; }
	void					SetLook(const _vec3& vLook);
public:
	void					SetSpeed(const _float& fSpeed);
public:
	HRESULT					Ready_Transform();
	HRESULT					SetUp_OnGraphicDev();
	void					Scaling(const _vec3& vScale);
	void					Scaling(const _float& fx, const _float& fy, const _float& fz);

	void					SetUp_Speed(const _float& fMovePerSec, const _float& fRotationPerSec);
	void					Go_Straight(const _float& fTimeDelta);
	void					Go_Left(const _float& fTimeDelta);
	void					Go_Right(const _float& fTimeDelta);
	void					Go_There(const _vec3& vPos);
	void					BackWard(const _float& fTimeDelta);
	void					SetUp_RotationX(const _float& fRadian);
	void					SetUp_RotationY(const _float& fRadian);
	void					SetUp_RotationZ(const _float& fRadian);
	void					Rotation_X(const _float& fTimeDelta);
	void					Rotation_Y(const _float& fTimeDelta);
	void					Rotation_Z(const _float& fTimeDelta);
	void					Rotation_Rev(const _float& fTimeDelta, CTransform* matWorld);
	void					Go_ToTarget(_vec3* pTargetPos, const _float& fTimeDelta);
	void					Rotation_Axis(const _float& fTimeDelta,const _vec3* pAxis);
private:
	_matrix					m_matWorld={}; // °´Ã¼ÀÇ »óÅÂ.
	_float					m_fSpeed_Move = 0.f;
	_float					m_fSpeed_Rotation=0.f;
	_float					m_fAdd_PosY = 0.f;
public:
	static CTransform*		Create();
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
};

