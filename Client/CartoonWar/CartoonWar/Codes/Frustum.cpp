#include "framework.h"
#include "Frustum.h"
#include "Transform.h"

CFrustum::CFrustum()
	: CComponent()
{
}

HRESULT CFrustum::Ready_Frustum()
{
	m_vOriginal_Point[0] = _vec3(-1.f, 1.f, 0.f);
	m_vOriginal_Point[1] = _vec3(1.f, 1.f, 0.f);
	m_vOriginal_Point[2] = _vec3(1.f, -1.f, 0.f);
	m_vOriginal_Point[3] = _vec3(-1.f, -1.f, 0.f);

	m_vOriginal_Point[4] = _vec3(-1.f, 1.f, 1.f);
	m_vOriginal_Point[5] = _vec3(1.f, 1.f, 1.f);
	m_vOriginal_Point[6] = _vec3(1.f, -1.f, 1.f);
	m_vOriginal_Point[7] = _vec3(-1.f, -1.f, 1.f);
	return S_OK;
}

HRESULT CFrustum::Transform_ToWorld()
{
	_matrix  matView, matProj;

	matView = CCamera_Manager::GetInstance()->GetMatView();
	matProj = CCamera_Manager::GetInstance()->GetMatProj();

	matView = Matrix_::Inverse(matView);
	matProj = Matrix_::Inverse(matProj);
	XMMATRIX xmMatView = ::XMLoadFloat4x4(&matView);
	XMMATRIX xmMatProj = ::XMLoadFloat4x4(&matProj);

	for (_uint i = 0; i < 8; ++i)
	{
		m_vPoint[i] = Vector3_::TransformCoord(m_vOriginal_Point[i], xmMatView);
		m_vPoint[i] = Vector3_::TransformCoord(m_vPoint[i], xmMatProj);
	}
	return S_OK;
}

_bool CFrustum::Culling_Frustum(CTransform* pTransform, const _float& fRadius)
{
	XMMATRIX	xmMatView, xmMatProj;
	_matrix  matView, matProj;

	matView = CCamera_Manager::GetInstance()->GetMatView();
	matProj = CCamera_Manager::GetInstance()->GetMatProj();

	_float	fZmin = -matProj._43 / matProj._33;
	_float	fR = g_Far / (g_Far - fZmin);

	matProj._33 = fR;
	matProj._43 = -fR * fZmin;

	xmMatView = XMLoadFloat4x4(&matView);
	xmMatProj = XMLoadFloat4x4(&matProj);

	XMMATRIX	matFinalMatrix = XMMatrixMultiply(xmMatView, xmMatProj);

	_matrix		matTemp;
	XMStoreFloat4x4(&matTemp, matFinalMatrix);

	// -Z
	_float	fX = (_float)(matTemp._14 + matTemp._13);
	_float	fY = (_float)(matTemp._24 + matTemp._23);
	_float	fZ = (_float)(matTemp._34 + matTemp._33);
	_float	fW = (_float)(matTemp._44 + matTemp._43);
	m_Plane[0] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[0] = XMPlaneNormalize(m_Plane[0]);

	// +Z
	fX = (_float)(matTemp._14 - matTemp._13);
	fY = (_float)(matTemp._24 - matTemp._23);
	fZ = (_float)(matTemp._34 - matTemp._33);
	fW = (_float)(matTemp._44 - matTemp._43);
	m_Plane[1] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[1] = XMPlaneNormalize(m_Plane[1]);

	// -X
	fX = (_float)(matTemp._14 + matTemp._11);
	fY = (_float)(matTemp._24 + matTemp._21);
	fZ = (_float)(matTemp._34 + matTemp._31);
	fW = (_float)(matTemp._44 + matTemp._41);
	m_Plane[2] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[2] = XMPlaneNormalize(m_Plane[2]);

	// +X
	fX = (_float)(matTemp._14 - matTemp._11);
	fY = (_float)(matTemp._24 - matTemp._21);
	fZ = (_float)(matTemp._34 - matTemp._31);
	fW = (_float)(matTemp._44 - matTemp._41);
	m_Plane[3] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[3] = XMPlaneNormalize(m_Plane[3]);


	// +Y
	fX = (_float)(matTemp._14 - matTemp._12);
	fY = (_float)(matTemp._24 - matTemp._22);
	fZ = (_float)(matTemp._34 - matTemp._32);
	fW = (_float)(matTemp._44 - matTemp._42);
	m_Plane[4] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[4] = XMPlaneNormalize(m_Plane[4]);


	// -Y
	fX = (_float)(matTemp._14 + matTemp._12);
	fY = (_float)(matTemp._24 + matTemp._22);
	fZ = (_float)(matTemp._34 + matTemp._32);
	fW = (_float)(matTemp._44 + matTemp._42);
	m_Plane[5] = XMVectorSet(fX, fY, fZ, fW);
	m_Plane[5] = XMPlaneNormalize(m_Plane[5]);



	_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	return Isin_Frustum(&vPos, fRadius);
}

_bool CFrustum::Isin_Frustum(const _vec3* pPosition, const _float& fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		XMVECTOR xmVector;

		_float fTemp = XMVectorGetX(XMPlaneDotCoord(m_Plane[i], XMVectorSet(pPosition->x, pPosition->y, pPosition->z, 1.f)));
		if (0.f > fTemp)
		{
			return false;
		}
	}
	return true;
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Ready_Frustum()))
	{
		MessageBox(0, L"CFrustum Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CFrustum::Clone_Component(void* pArg)
{
	return new CFrustum(*this);
}

void CFrustum::Free()
{
	CComponent::Free();
}
