#include "stdafx.h"
#include "Transform.h"


CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
	m_vRotate = _vec3(0.f, 0.f, 0.f);
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
{
	m_vRotate = _vec3(0.f,0.f,0.f);
}

const _vec3 * CTransform::Get_StateInfo(STATE eState) const
{
	return (_vec3*)&m_matWorld.m[eState][0];
}

_vec3 CTransform::Get_Scale()
{
	_float		fScaleX = D3DXVec3Length(Get_StateInfo(STATE_RIGHT));
	_float		fScaleY = D3DXVec3Length(Get_StateInfo(STATE_UP));
	_float		fScaleZ = D3DXVec3Length(Get_StateInfo(STATE_LOOK));

	return _vec3(fScaleX, fScaleY, fScaleZ);
}

_matrix CTransform::Get_Matrix_Inverse() const
{
	_matrix			matInverse;
	D3DXMatrixInverse(&matInverse, nullptr, &m_matWorld);

	return _matrix(matInverse);
}

void CTransform::Set_StateInfo(STATE eState, const _vec3 * pInfo)
{
	memcpy(&m_matWorld.m[eState][0], pInfo, sizeof(_vec3));

}

void CTransform::Set_Rotate(_vec3 vRot)
{
	m_vRotate = vRot;
	Rotation_X(D3DXToRadian(m_vRotate.x));
	Rotation_Y(D3DXToRadian(m_vRotate.y));
	Rotation_Z(D3DXToRadian(m_vRotate.z));
}

HRESULT CTransform::Ready_Transform()
{
	D3DXMatrixIdentity(&m_matWorld);

	return NOERROR;
}

HRESULT CTransform::SetUp_OnGraphicDev()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_matWorld);

	return NOERROR;
}

HRESULT CTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName)
{
	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->SetMatrix(pConstantName, &m_matWorld);
	return S_OK;
}

void CTransform::SetUp_Speed(const _float & fMovePerSec, const _float & fRotationPerSec)
{
	m_fSpeed_Move = fMovePerSec;
	m_fSpeed_Rotation = fRotationPerSec;
}

void CTransform::Go_Straight(const _float & fTimeDelta)
{
	_vec3	vLook, vPosition;

	vLook = *Get_StateInfo(STATE_LOOK);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed_Move * fTimeDelta;

	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_Left(const _float & fTimeDelta)
{
	_vec3	vRight, vPosition;

	vRight = *Get_StateInfo(STATE_RIGHT);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vPosition -= *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed_Move * fTimeDelta;

	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::Go_Right(const _float & fTimeDelta)
{
	_vec3	vRight, vPosition;

	vRight = *Get_StateInfo(STATE_RIGHT);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed_Move * fTimeDelta;

	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::BackWard(const _float & fTimeDelta)
{
	_vec3	vLook, vPosition;

	vLook = *Get_StateInfo(STATE_LOOK);
	vPosition = *Get_StateInfo(STATE_POSITION);

	vPosition -= *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed_Move * fTimeDelta;

	Set_StateInfo(STATE_POSITION, &vPosition);
}

void CTransform::SetUp_RotationX(const _float & fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix			matRot;
	D3DXMatrixRotationX(&matRot, fRadian);

	vRight *= Get_Scale().x;
	vUp *= Get_Scale().y;
	vLook *= Get_Scale().z;


	D3DXVec3TransformNormal(&vRight, &vRight, &matRot);
	D3DXVec3TransformNormal(&vUp, &vUp, &matRot);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}

void CTransform::SetUp_RotationY(const _float & fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix			matRot;
	D3DXMatrixRotationY(&matRot, fRadian);

	vRight *= Get_Scale().x;
	vUp *= Get_Scale().y;
	vLook *= Get_Scale().z;


	D3DXVec3TransformNormal(&vRight, &vRight, &matRot);
	D3DXVec3TransformNormal(&vUp, &vUp, &matRot);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}

void CTransform::SetUp_RotationZ(const _float & fRadian)
{
	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	_matrix			matRot;
	D3DXMatrixRotationZ(&matRot, fRadian);

	vRight *= Get_Scale().x;
	vUp *= Get_Scale().y;
	vLook *= Get_Scale().z;


	D3DXVec3TransformNormal(&vRight, &vRight, &matRot);
	D3DXVec3TransformNormal(&vUp, &vUp, &matRot);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	Set_StateInfo(STATE_RIGHT, &vRight);
	Set_StateInfo(STATE_UP, &vUp);
	Set_StateInfo(STATE_LOOK, &vLook);
}

void CTransform::Rotation_X(const _float & fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix			matRot;
	D3DXMatrixRotationX(&matRot, m_fSpeed_Rotation * fTimeDelta);

	for (size_t i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(&vDir[i], &vDir[i], &matRot);

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Rotation_Y(const _float & fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix			matRot;
	D3DXMatrixRotationY(&matRot, m_fSpeed_Rotation * fTimeDelta);

	for (size_t i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(&vDir[i], &vDir[i], &matRot);

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Rotation_Z(const _float & fTimeDelta)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix			matRot;
	D3DXMatrixRotationZ(&matRot, m_fSpeed_Rotation * fTimeDelta);

	for (size_t i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(&vDir[i], &vDir[i], &matRot);

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Rotation_Axis(const _float & fTimeDelta, const _vec3 * pAxis)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
		vDir[i] = *Get_StateInfo(STATE(i));

	_matrix			matRot;
	D3DXMatrixRotationAxis(&matRot, pAxis, m_fSpeed_Rotation * fTimeDelta);

	for (size_t i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(&vDir[i], &vDir[i], &matRot);

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Scaling(const _float & fX, const _float & fY, const _float & fZ)
{
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
	{
		vDir[i] = *Get_StateInfo(STATE(i));
		D3DXVec3Normalize(&vDir[i], &vDir[i]);
	}

	vDir[STATE_RIGHT] *= fX;
	vDir[STATE_UP] *= fY;
	vDir[STATE_LOOK] *= fZ;

	Set_StateInfo(STATE_RIGHT, &vDir[STATE_RIGHT]);
	Set_StateInfo(STATE_UP, &vDir[STATE_UP]);
	Set_StateInfo(STATE_LOOK, &vDir[STATE_LOOK]);
}

void CTransform::Go_ToTarget(const _vec3 * pTargetPos, const _float & fTimeDelta)
{
	_vec3	vLook, vPosition;

	vPosition = *Get_StateInfo(STATE_POSITION);

	vLook = *pTargetPos - vPosition;

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed_Move * fTimeDelta;

	Set_StateInfo(STATE_POSITION, &vPosition);
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform*	pInstance = new CTransform(pGraphic_Device);

	if (FAILED(pInstance->Ready_Transform()))
	{
		MessageBox(0, L"CTransform Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent * CTransform::Clone_Component(void* pArg)
{
	return new CTransform(*this);
}

void CTransform::Free()
{
	CComponent::Free();
}

