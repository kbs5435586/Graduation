#include "framework.h"
#include "Frustum.h"
#include "Transform.h"

CFrustum::CFrustum(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
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

	matView = CDevice::GetInstance()->GetViewMatrix();
	matProj = CDevice::GetInstance()->GetProjectionMatrix();

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
	_vec3		vPoint[8];
	memcpy(vPoint, m_vPoint, sizeof(_vec3) * 8);

	_vec3		vPosition = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	_matrix		matWorldInv = pTransform->Get_Matrix_Inverse();
	XMMATRIX	xmMatWorldInv = ::XMLoadFloat4x4(&matWorldInv);

	vPosition = Vector3_::TransformCoord(vPosition, xmMatWorldInv);
	
	for (_uint i = 0; i < 8; ++i)
		vPoint[i] = Vector3_::TransformCoord(vPoint[i], xmMatWorldInv);



	return _bool();
}

_bool CFrustum::Isin_Frustum(const _vec3* pPosition, const _float& fRadius)
{
	return _bool();
}

CFrustum* CFrustum::Create(ID3D12Device* pGraphic_Device)
{
	return nullptr;
}

CComponent* CFrustum::Clone_Component(void* pArg)
{
	return nullptr;
}

void CFrustum::Free()
{
}
