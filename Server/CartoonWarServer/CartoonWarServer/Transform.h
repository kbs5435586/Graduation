#pragma once
class CTransform
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
public:
	CTransform();
	~CTransform();
public: // Getter
	_vec3*					Get_StateInfo(STATE eState);
	_vec3					Get_Scale();
	_matrix					Get_Matrix_Inverse() const;
	_matrix					Get_Matrix() { return m_matWorld; }
public: // Setter
	void					Set_StateInfo(STATE eState, const _vec3* pInfo);
	void					Set_PositionY(const _float& fY) { m_matWorld.m[3][1] = fY; }
	void					Set_Matrix(_matrix* mat);
public:
	HRESULT					Ready_Transform();
	HRESULT					SetUp_OnGraphicDev();

	void					SetUp_Speed(const _float& fMovePerSec, const _float& fRotationPerSec);
	void					Go_Straight(const _float& fTimeDelta);
	void					Go_Left(const _float& fTimeDelta);
	void					Go_Right(const _float& fTimeDelta);
	void					BackWard(const _float& fTimeDelta);
	void					SetUp_RotationX(const _float& fRadian);
	void					SetUp_RotationY(const _float& fRadian);
	void					SetUp_RotationZ(const _float& fRadian);
	void					Rotation_X(const _float& fTimeDelta);
	void					Rotation_Y(const _float& fTimeDelta);
	void					Rotation_Z(const _float& fTimeDelta);
	void					Scaling(const _float& fX, const _float& fY, const _float& fZ);
	void					Go_ToTarget(_vec3* pTargetPos, const _float& fTimeDelta);
	void					Rotation_Axis(const _float& fTimeDelta,const _vec3* pAxis);
private:
	_matrix					m_matWorld={}; // ��ü�� ����.
	_float					m_fSpeed_Move = 1.f;
	_float					m_fSpeed_Rotation = 1.f;
};

