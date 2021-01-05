#include "stdafx.h"
#include "Buffer_RcTex.h"

CBuffer_RcTex::CBuffer_RcTex(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_RcTex::CBuffer_RcTex(const CBuffer_RcTex & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_RcTex::Ready_VIBuffer()
{
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_pPosition = new _vec3[4];

	m_iNumPolygons = 2;
	m_iPolygonSize = sizeof(POLYGON16);
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _vec2(1.f, 0.f);

	pVertices[2].vPosition = _vec3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _vec2(1.f, 1.f);

	pVertices[3].vPosition = _vec3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _vec2(0.f, 1.f);

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = pVertices[i].vPosition;

	m_pVB->Unlock();




	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CBuffer_RcTex::Render_VIBuffer()
{
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
}

_bool CBuffer_RcTex::Picking_ToBuffer(_vec3 * pOut, const CTransform * pTransformCom, const CPicking * pPickingCom)
{
	_vec3	vRayPivot = pPickingCom->Get_MouseRayPivot();
	_vec3	vRay = pPickingCom->Get_MouseRay();

	_matrix		matWorldInv = pTransformCom->Get_Matrix_Inverse();

	D3DXVec3TransformCoord(&vRayPivot, &vRayPivot, &matWorldInv);
	D3DXVec3TransformNormal(&vRay, &vRay, &matWorldInv);

	_float fU, fV, fDist;
	_bool	isPick = false;

	if (D3DXIntersectTri(&m_pPosition[1], &m_pPosition[0], &m_pPosition[2], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;
	if (D3DXIntersectTri(&m_pPosition[3], &m_pPosition[2], &m_pPosition[0], &vRayPivot, &vRay, &fU, &fV, &fDist))
		isPick = true;

	return isPick;
}

CBuffer_RcTex * CBuffer_RcTex::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuffer_RcTex*	pInstance = new CBuffer_RcTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_RcTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CBuffer_RcTex::Clone_Component(void* pArg)
{
	if(nullptr != pArg)
		m_pComponent_Tag = (_tchar*)pArg;
	return new CBuffer_RcTex(*this);
}

void CBuffer_RcTex::Free()
{
	CVIBuffer::Free();
}
