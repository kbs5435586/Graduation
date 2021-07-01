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

	matView = matView.Invert();
	matProj = matProj.Invert();
	XMMATRIX xmMatView = ::XMLoadFloat4x4(&matView);
	XMMATRIX xmMatProj = ::XMLoadFloat4x4(&matProj);

	for (_uint i = 0; i < 8; ++i)
	{
		m_vPoint[i] = _vec3::Transform(m_vOriginal_Point[i], xmMatProj);
		m_vPoint[i] = _vec3::Transform(m_vPoint[i], xmMatView);
	}
	return S_OK;
}

_bool CFrustum::Culling_Frustum(CTransform* pTransform, const _float& fRadius)
{
	_vec3 vPoint[8];
	memcpy(vPoint, m_vPoint, sizeof(_vec3) * 8);


	_vec3	vPosition = *(pTransform->Get_StateInfo(CTransform::STATE_POSITION));
	_matrix	matWorldInv = pTransform->Get_Matrix_Inverse();

	XMMATRIX temp = XMLoadFloat4x4(&matWorldInv);
	vPosition = _vec3::Transform(vPosition, temp);

	for (_uint i = 0; i < 8; ++i)
		vPoint[i] = _vec3::Transform(vPoint[i], temp);


	m_Plane[0] = Plane(vPoint[0], vPoint[1], vPoint[2]);
	m_Plane[1] = Plane(vPoint[5], vPoint[4], vPoint[7]);

	m_Plane[2] = Plane(vPoint[4], vPoint[5], vPoint[1]);
	m_Plane[3] = Plane(vPoint[6], vPoint[7], vPoint[2]);

	m_Plane[4] = Plane(vPoint[4], vPoint[0], vPoint[7]);
	m_Plane[5] = Plane(vPoint[5], vPoint[6], vPoint[1]);


	return Isin_Frustum(m_Plane, &vPosition, fRadius);

}

_bool CFrustum::Isin_Frustum(Plane* pPlane, const _vec3* pPosition, const _float& fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{

		_vec4 vNormal = pPlane[i];
		_vec3 vTemp = _vec3(vNormal.x, vNormal.y, vNormal.z);
		_vec4 vTemp_Pos = _vec4(pPosition->x, pPosition->y, pPosition->z,1.f);
		if (vNormal.Dot(vTemp_Pos) + pPlane[i].w > fRadius)
			return false;
	}
	return true;
}

HRESULT CFrustum::Get_LocalPlane(Plane* pOutPlane, const _matrix* pInMatWorld)
{
	_vec3	vPoint[8];
	_matrix	matWorldInv;


	matWorldInv = matWorldInv.Invert();

	XMMATRIX temp = XMLoadFloat4x4(&matWorldInv);
	for (_uint i = 0; i < 8; ++i)
	{
		vPoint[i]= _vec3::Transform(vPoint[i], temp);
	}
	pOutPlane[0] = Plane(vPoint[1], vPoint[5], vPoint[6]);
	pOutPlane[1] = Plane(vPoint[4], vPoint[0], vPoint[3]);

	pOutPlane[2] = Plane(vPoint[4], vPoint[5], vPoint[1]);
	pOutPlane[3] = Plane(vPoint[3], vPoint[2], vPoint[6]);

	pOutPlane[4] = Plane(vPoint[7], vPoint[6], vPoint[5]);
	pOutPlane[5] = Plane(vPoint[0], vPoint[1], vPoint[2]);

	return S_OK;
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
	AddRef();

	return this;
}

void CFrustum::Free()
{
	CComponent::Free();
}
