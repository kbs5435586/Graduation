#include "framework.h"
#include "QuadTree.h"
#include "Management.h"
#include "Frustum.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Ready_QuadTree(_vec3* pVerticesPos, const _uint& iNumVerticeX, const _uint& iNumVerticeZ)
{
	m_pVerticesPos = pVerticesPos;

	m_iCorner[(_uint)CORNER::CORNER_LT] = iNumVerticeX * iNumVerticeZ - iNumVerticeX;
	m_iCorner[(_uint)CORNER::CORNER_RT] = iNumVerticeX * iNumVerticeZ - 1;
	m_iCorner[(_uint)CORNER::CORNER_RB] = iNumVerticeX - 1;
	m_iCorner[(_uint)CORNER::CORNER_LB] = 0;

	m_iCenter = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_RT]) >> 1;

	_vec3 vTemp = m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]] - m_pVerticesPos[m_iCenter];
	m_fRadius = Vector3_::Length(vTemp);

	SetUp_ChildNode();

	return S_OK;
}

HRESULT CQuadTree::Ready_QuadTree(_vec3* pVerticesPos, const _uint& iLT, const _uint& iRT, const _uint& iRB, const _uint& iLB)
{
	m_pVerticesPos = pVerticesPos;

	m_iCorner[(_uint)CORNER::CORNER_LT] = iLT;
	m_iCorner[(_uint)CORNER::CORNER_RT] = iRT;
	m_iCorner[(_uint)CORNER::CORNER_RB] = iRB;
	m_iCorner[(_uint)CORNER::CORNER_LB] = iLB;


	_vec3 vTemp = m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]] - m_pVerticesPos[m_iCenter];
	m_fRadius = Vector3_::Length(vTemp);
	return S_OK;
}

void CQuadTree::Culling_ToQuadTree(CFrustum* pFrustum, Plane* pLocalPlane, const _uint& pVertexIndex, _uint& iNumPolygons)
{
}

_bool CQuadTree::Check_LOD()
{
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	matView = Matrix_::Inverse(matView);


	_vec3			vCamPosition;
	memcpy(&vCamPosition, &matView.m[3][0], sizeof(_vec3));

	_vec3 vTempDist = vCamPosition - m_pVerticesPos[m_iCenter];
	_vec3 vTempWidth = m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_RT]] - m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]];
	_float		fDistance = Vector3_::Length(vTempDist);

	_float		fWidth = Vector3_::Length(vTempWidth);

	if (fDistance * 0.1f > fWidth)
		return true;

	return _bool(false);
}

HRESULT CQuadTree::SetUp_ChildNode()
{
	if (1 == m_iCorner[(_uint)CORNER::CORNER_RT] - m_iCorner[(_uint)CORNER::CORNER_LT])
		return NOERROR;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_LT]) >> 1;
	iTC = (m_iCorner[(_uint)CORNER::CORNER_RT] + m_iCorner[(_uint)CORNER::CORNER_LT]) >> 1;
	iRC = (m_iCorner[(_uint)CORNER::CORNER_RB] + m_iCorner[(_uint)CORNER::CORNER_RT]) >> 1;
	iBC = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_RB]) >> 1;

	m_pChild[(_uint)CHILD::CHILD_LT] = CQuadTree::Create(m_pVerticesPos, m_iCorner[(_uint)CORNER::CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[(_uint)CHILD::CHILD_RT] = CQuadTree::Create(m_pVerticesPos, iTC, m_iCorner[(_uint)CORNER::CORNER_RT], iRC, m_iCenter);
	m_pChild[(_uint)CHILD::CHILD_RB] = CQuadTree::Create(m_pVerticesPos, m_iCenter, iRC, m_iCorner[(_uint)CORNER::CORNER_RB], iBC);
	m_pChild[(_uint)CHILD::CHILD_LB] = CQuadTree::Create(m_pVerticesPos, iLC, m_iCenter, iBC, m_iCorner[(_uint)CORNER::CORNER_LB]);

	for (size_t i = 0; i < (_uint)CHILD::CHILD_END; i++)
	{
		m_pChild[i]->SetUp_ChildNode();
	}

	return S_OK;
}

CQuadTree* CQuadTree::Create(_vec3* pVerticesPos, const _uint& iNumVerticeX, const _uint& iNumVerticeZ)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Ready_QuadTree(pVerticesPos, iNumVerticeX, iNumVerticeZ)))
	{
		MessageBox(0, L"CQuadTree Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CQuadTree* CQuadTree::Create(_vec3* pVerticesPos, const _uint& iLT, const _uint& iRT, const _uint& iRB, const _uint& iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Ready_QuadTree(pVerticesPos, iLT, iRT, iRB, iLB)))
	{
		MessageBox(0, L"CQuadTree ChildNode Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (size_t i = 0; i < (_uint)CHILD::CHILD_END; i++)
		Safe_Release(m_pChild[i]);

}
