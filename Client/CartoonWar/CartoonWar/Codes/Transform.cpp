#include "framework.h"
#include "Transform.h"

CTransform::CTransform()
{

}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)

{
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
}


void CTransform::SetLook(const _vec3& vLook_)
{
	_vec3 vLook = vLook_;
	_vec3 vUp = { 0.f,1.f,0.f };
	_vec3 vRight = {};
	vLook = Vector3_::Normalize(vLook);
	vRight = Vector3_::CrossProduct(vUp, vLook);
	vUp = Vector3_::CrossProduct(vLook, vRight);

	this->Set_StateInfo(STATE::STATE_RIGHT, &vRight);
	this->Set_StateInfo(STATE::STATE_UP, &vUp);
	this->Set_StateInfo(STATE::STATE_LOOK, &vLook);
	this->Scaling(m_vScale);
}

void CTransform::SetSpeed(const _float& fSpeed)
{
	m_fSpeed_Move = fSpeed;
}


HRESULT CTransform::Ready_Transform()
{
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	return S_OK;
}

HRESULT CTransform::SetUp_OnGraphicDev()
{
	return S_OK;
}


_vec3* CTransform::Get_StateInfo(STATE eState)
{
	return (_vec3*)&m_matWorld.m[eState][0];
}

_vec3 CTransform::Get_Scale()
{
	_float			fScaleX = Vector3_::Length(*Get_StateInfo(STATE_RIGHT));
	_float			fScaleY = Vector3_::Length(*Get_StateInfo(STATE_UP));
	_float			fScaleZ = Vector3_::Length(*Get_StateInfo(STATE_LOOK));

	return _vec3(fScaleX, fScaleY, fScaleZ);
}

_matrix CTransform::Get_Matrix_Inverse() const
{
	//XMFLOAT4X4
	_matrix mat = m_matWorld;
	mat = Matrix_::Inverse(mat);
	return mat;
}

void CTransform::Set_Matrix(_matrix matWorld, _bool IsMul)
{
	m_matWorld = Matrix_::Multiply(m_matWorld, matWorld);
}

void CTransform::Set_StateInfo(STATE eState, const _vec3* pInfo)
{
	memcpy(&m_matWorld.m[eState][0], pInfo, sizeof(_vec3));
}


void CTransform::SetUp_Speed(const _float& fMovePerSec, const _float& fRotationPerSec)
{
	m_fSpeed_Move = fMovePerSec;
	m_fSpeed_Rotation = fRotationPerSec;
}

void CTransform::Go_Straight(const _float& fTimeDelta)
{
	_vec3		vLook, vPosition;
	vLook = *Get_StateInfo(STATE_LOOK);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = Vector3_::Normalize(vLook);
	vLook = Vector3_::ScalarProduct(vLook, m_fSpeed_Move * fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vLook);
	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_Left(const _float& fTimeDelta)
{
	_vec3		vRight, vPosition;

	vRight = *Get_StateInfo(STATE_RIGHT);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vRight = Vector3_::Normalize(vRight);
	vRight = Vector3_::ScalarProduct(vRight, m_fSpeed_Move * -fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vRight);
	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_Right(const _float& fTimeDelta)
{
	_vec3		vRight, vPosition;

	vRight = *Get_StateInfo(STATE_RIGHT);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vRight = Vector3_::Normalize(vRight);
	vRight = Vector3_::ScalarProduct(vRight, m_fSpeed_Move * fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vRight);
	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_There(const _vec3& vPos)
{
	m_matWorld._41 -= (vPos.x);
	m_matWorld._42 -= (vPos.y);
	m_matWorld._43 -= (vPos.z);
}

void CTransform::Go_MouseMoveLR(const _float& fTimeDelta)
{
	_vec3		vRight, vPosition;

	vRight = *Get_StateInfo(STATE_RIGHT);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vRight = Vector3_::Normalize(vRight);
	vRight = Vector3_::ScalarProduct(vRight, (m_fSpeed_Move/3) * -fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vRight);
	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_MouseMoveUD(const _float& fTimeDelta)
{
	_vec3		vUp, vPosition;
	vUp = *Get_StateInfo(STATE_UP);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vUp = Vector3_::Normalize(vUp);
	vUp = Vector3_::ScalarProduct(vUp, (m_fSpeed_Move / 6) * -fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vUp);
	Set_StateInfo(CTransform::STATE_POSITION, &vPosition);
}

void CTransform::BackWard(const _float& fTimeDelta)
{
	_vec3		vLook, vPosition;
	vLook = *Get_StateInfo(STATE_LOOK);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = Vector3_::Normalize(vLook);
	vLook = Vector3_::ScalarProduct(vLook, m_fSpeed_Move * -fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vLook);
	Set_StateInfo(STATE_POSITION, &vPosition);
}
void CTransform::UP(const _float& fTimeDelta)
{
	float gravity = 9.8f;

	float t = fTimeDelta;
	float tt = fTimeDelta * fTimeDelta;

	float accY = 0;

	//Jump!
	accY += gravity;
	m_fVel = m_fVel + accY * t;

	_vec3		vLook, vPosition;
	vLook = *Get_StateInfo(STATE_UP);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = Vector3_::Normalize(vLook);
	vLook = Vector3_::ScalarProduct(vLook, m_fVel * t + accY * tt, false);
	vPosition = Vector3_::Add(vPosition, vLook);
	Set_StateInfo(STATE_POSITION, &vPosition);
}
void CTransform::Fallen(const _float& fTimeDelta)
{
	float gravity = 9.8f;

	float t = fTimeDelta;
	float tt = fTimeDelta * fTimeDelta;

	
	float accY = 0;
	
	accY -= gravity;
	m_fVel = m_fVel + accY * t;

	//m_PositionY = m_PositionY + myVel * t + 0.5f * accY * tt;


	_vec3		vLook, vPosition;
	vLook = *Get_StateInfo(STATE_UP);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = Vector3_::Normalize(vLook);
	vLook = Vector3_::ScalarProduct(vLook, m_fVel * t + accY * tt , false);
	vPosition = Vector3_::Add(vPosition, vLook);
	Set_StateInfo(STATE_POSITION, &vPosition);

}
void CTransform::SetUp_RotationX(const _float& fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix		matRot = Matrix_::Identity();
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationX(fRadian)));

	vRight = Vector3_::ScalarProduct(vRight, Get_Scale().x, false);
	vUp = Vector3_::ScalarProduct(vUp, Get_Scale().y, false);
	vLook = Vector3_::ScalarProduct(vLook, Get_Scale().z, false);

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);

	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}
void CTransform::Rotation_Axis(const _float& fTimeDelta, const _vec3* pAxis)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix			matRot;
	XMVECTOR vAxis = ::XMLoadFloat3(pAxis);

	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis(vAxis, m_fSpeed_Rotation * fTimeDelta)));

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);
	for (size_t i = 0; i < 3; ++i)
		vDir[i] = Vector3_::TransformNormal(vDir[i], mat);

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}
void CTransform::SetUp_RotationY(const _float& fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix		matRot = Matrix_::Identity();
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(fRadian)));

	vRight = Vector3_::ScalarProduct(vRight, Get_Scale().x, false);
	vUp = Vector3_::ScalarProduct(vUp, Get_Scale().y, false);
	vLook = Vector3_::ScalarProduct(vLook, Get_Scale().z, false);

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);

	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}

void CTransform::SetUp_RotationZ(const _float& fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix		matRot = Matrix_::Identity();
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationZ(fRadian)));

	vRight = Vector3_::ScalarProduct(vRight, Get_Scale().x, false);
	vUp = Vector3_::ScalarProduct(vUp, Get_Scale().y, false);
	vLook = Vector3_::ScalarProduct(vLook, Get_Scale().z, false);

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);

	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}

void CTransform::Rotation_X(const _float& fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix		matRot;
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationX(m_fSpeed_Rotation * fTimeDelta)));

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);
	for (size_t i = 0; i < 3; ++i)
		vDir[i] = Vector3_::TransformNormal(vDir[i], mat);


	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Rotation_Y(const _float& fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix		matRot;
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(m_fSpeed_Rotation * -fTimeDelta)));

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);
	for (size_t i = 0; i < 3; ++i)
		vDir[i] = Vector3_::TransformNormal(vDir[i], mat);


	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Rotation_Z(const _float& fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix		matRot;
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationZ(m_fSpeed_Rotation * -fTimeDelta)));

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);
	for (size_t i = 0; i < 3; ++i)
		vDir[i] = Vector3_::TransformNormal(vDir[i], mat);


	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);

	mat.r;
}

void CTransform::Rotation_Rev(const _float& fTimeDelta, CTransform* matWorld)
{
	_matrix		matTemp= matWorld->Get_Matrix();
	_vec3		vDir[3];
	for (size_t i = 0; i < 3; ++i)
	{
		vDir[i] = *matWorld->Get_StateInfo(CTransform::STATE(i));
		vDir[i] = Vector3_::Normalize(vDir[i]);
	}
	vDir[0] = Vector3_::ScalarProduct(vDir[0], matWorld->Get_Scale().x, false);
	vDir[1] = Vector3_::ScalarProduct(vDir[1],  matWorld->Get_Scale().y, false);
	vDir[2] = Vector3_::ScalarProduct(vDir[2],  matWorld->Get_Scale().z, false);

	matTemp._11 = vDir[0].x;
	matTemp._12 = vDir[0].y;
	matTemp._13 = vDir[0].z;

	matTemp._21 = vDir[1].x;
	matTemp._22 = vDir[1].y;
	matTemp._23 = vDir[1].z;

	matTemp._31 = vDir[2].x;
	matTemp._32 = vDir[2].y;
	matTemp._33 = vDir[2].z;

	matTemp._44 = 1.f;

	_matrix matTemp2 = {};
	matTemp2._41 = m_matWorld._41;
	matTemp2._42 = 20.f;
	matTemp2._43 = m_matWorld._43;

	_matrix		matRev;
	DirectX::XMStoreFloat4x4(&matRev, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(m_fSpeed_Rotation * -fTimeDelta)));


	m_matWorld = matTemp2* matRev * matTemp;
	Set_Matrix(m_matWorld);
}


void CTransform::Scaling(const _vec3& vScale)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
	{
		vDir[i] = *Get_StateInfo(STATE(i));
		vDir[i] = Vector3_::Normalize(vDir[i]);
	}


	vDir[STATE_RIGHT] = Vector3_::ScalarProduct(vDir[STATE_RIGHT], vScale.x, false);
	vDir[STATE_UP] = Vector3_::ScalarProduct(vDir[STATE_UP], vScale.y, false);
	vDir[STATE_LOOK] = Vector3_::ScalarProduct(vDir[STATE_LOOK], vScale.z, false);


	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);

	m_vScale = vScale;
}

void CTransform::Scaling(const _float& fx, const _float& fy, const _float& fz)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
	{
		vDir[i] = *Get_StateInfo(STATE(i));
		vDir[i] = Vector3_::Normalize(vDir[i]);
	}

	_vec3 vScale = _vec3(fx, fy, fz);
	vDir[STATE_RIGHT] = Vector3_::ScalarProduct(vDir[STATE_RIGHT], vScale.x, false);
	vDir[STATE_UP] = Vector3_::ScalarProduct(vDir[STATE_UP], vScale.y, false);
	vDir[STATE_LOOK] = Vector3_::ScalarProduct(vDir[STATE_LOOK], vScale.z, false);


	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
	m_vScale = vScale;
}

void CTransform::Go_ToTarget(_vec3* pTargetPos, const _float& fTimeDelta)
{
	_vec3		vLook, vPosition;
	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = Vector3_::Subtract(*pTargetPos, vPosition);
	vLook = Vector3_::Normalize(vLook);
	vLook = Vector3_::ScalarProduct(vLook, m_fSpeed_Move * fTimeDelta, false);
	vPosition = Vector3_::Add(vPosition, vLook);
	Set_StateInfo(STATE_POSITION, &vPosition);
}


CTransform* CTransform::Create()
{
	CTransform* pInstance = new CTransform();

	if (FAILED(pInstance->Ready_Transform()))
	{
		MessageBox(0, L"CTransform Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone_Component(void* pArg)
{
	return new CTransform(*this);
}


void CTransform::Free()
{
	CComponent::Free();
}
