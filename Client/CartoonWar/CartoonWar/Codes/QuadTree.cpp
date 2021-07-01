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
	m_fRadius = vTemp.Length();

	SetUp_ChildNode();
	SetUp_Neighbor();

	return S_OK;
}

HRESULT CQuadTree::Ready_QuadTree(_vec3* pVerticesPos, const _uint& iLT, const _uint& iRT, const _uint& iRB, const _uint& iLB)
{
	m_pVerticesPos = pVerticesPos;

	m_iCorner[(_uint)CORNER::CORNER_LT] = iLT;
	m_iCorner[(_uint)CORNER::CORNER_RT] = iRT;
	m_iCorner[(_uint)CORNER::CORNER_RB] = iRB;
	m_iCorner[(_uint)CORNER::CORNER_LB] = iLB;

	m_iCenter = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_RT]) >> 1;

	_vec3 vTemp = m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]] - m_pVerticesPos[m_iCenter];
	m_fRadius = vTemp.Length();
	return S_OK;
}

void CQuadTree::Culling_ToQuadTree(CFrustum* pFrustum, Plane* pLocalPlane, _uint* pVertexIndex, _uint& iNumPolygons)
{
	if (nullptr == m_pChild[(_uint)CHILD::CHILD_LT] ||
		true == Check_LOD())
	{


		_bool		isIn[(_uint)CORNER::CORNER_END] = { false };

		isIn[(_uint)CORNER::CORNER_LT] = pFrustum->Isin_Frustum(pLocalPlane, &m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]], 0.f);
		isIn[(_uint)CORNER::CORNER_RT] = pFrustum->Isin_Frustum(pLocalPlane, &m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_RT]], 0.f);
		isIn[(_uint)CORNER::CORNER_RB] = pFrustum->Isin_Frustum(pLocalPlane, &m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_RB]], 0.f);
		isIn[(_uint)CORNER::CORNER_LB] = pFrustum->Isin_Frustum(pLocalPlane, &m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LB]], 0.f);


		_bool		isLOD[(_uint)NEIGHBOR::NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < (_uint)NEIGHBOR::NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbor[i])
				isLOD[i] = m_pNeighbor[i]->Check_LOD();
		}

		if (true == isLOD[(_uint)NEIGHBOR::NEIGHBOR_LEFT] &&
			true == isLOD[(_uint)NEIGHBOR::NEIGHBOR_TOP] &&
			true == isLOD[(_uint)NEIGHBOR::NEIGHBOR_RIGHT] &&
			true == isLOD[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM])
		{
			if (true == isIn[(_uint)CORNER::CORNER_LT] ||
				true == isIn[(_uint)CORNER::CORNER_RT] ||
				true == isIn[(_uint)CORNER::CORNER_RB])
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons+1] = m_iCorner[(_uint)CORNER::CORNER_RT];
				pVertexIndex[iNumPolygons+2] = m_iCorner[(_uint)CORNER::CORNER_RB];
				iNumPolygons += 3;
			}

			if (true == isIn[(_uint)CORNER::CORNER_LT] ||
				true == isIn[(_uint)CORNER::CORNER_RB] ||
				true == isIn[(_uint)CORNER::CORNER_LB])
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons+1] = m_iCorner[(_uint)CORNER::CORNER_RB];
				pVertexIndex[iNumPolygons+2] = m_iCorner[(_uint)CORNER::CORNER_LB];
				iNumPolygons += 3;
			}
			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_LT]) >> 1;
		iTC = (m_iCorner[(_uint)CORNER::CORNER_RT] + m_iCorner[(_uint)CORNER::CORNER_LT]) >> 1;
		iRC = (m_iCorner[(_uint)CORNER::CORNER_RB] + m_iCorner[(_uint)CORNER::CORNER_RT]) >> 1;
		iBC = (m_iCorner[(_uint)CORNER::CORNER_LB] + m_iCorner[(_uint)CORNER::CORNER_RB]) >> 1;

		if (true == isIn[(_uint)CORNER::CORNER_LT] ||
			true == isIn[(_uint)CORNER::CORNER_RB] ||
			true == isIn[(_uint)CORNER::CORNER_LB])
		{
			if (false == isLOD[(_uint)NEIGHBOR::NEIGHBOR_LEFT])
			{

				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons+1] = m_iCenter;
				pVertexIndex[iNumPolygons+2] = iLC;
	
				
				pVertexIndex[iNumPolygons+3] = iLC;
				pVertexIndex[iNumPolygons+4] = m_iCenter;
				pVertexIndex[iNumPolygons+5] = m_iCorner[(_uint)CORNER::CORNER_LB];
				iNumPolygons+=6;

			}
			else
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons+1] = m_iCenter;
				pVertexIndex[iNumPolygons+2] = m_iCorner[(_uint)CORNER::CORNER_LB];
				iNumPolygons+=3;
			}

			if (false == isLOD[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM])
			{

				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LB];
				pVertexIndex[iNumPolygons+1] = m_iCenter;
				pVertexIndex[iNumPolygons+2] = iBC;

				pVertexIndex[iNumPolygons+3] = iBC;
				pVertexIndex[iNumPolygons+4] = m_iCenter;
				pVertexIndex[iNumPolygons+5] = m_iCorner[(_uint)CORNER::CORNER_RB];
				iNumPolygons+=6;

			}
			else
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LB];
				pVertexIndex[iNumPolygons + 1] = m_iCenter;
				pVertexIndex[iNumPolygons + 2] = m_iCorner[(_uint)CORNER::CORNER_RB];
				iNumPolygons += 3;
			}
		}

		if (true == isIn[(_uint)CORNER::CORNER_LT] ||
			true == isIn[(_uint)CORNER::CORNER_RT] ||
			true == isIn[(_uint)CORNER::CORNER_RB])
		{
			if (false == isLOD[(_uint)NEIGHBOR::NEIGHBOR_TOP])
			{
				pVertexIndex[iNumPolygons]= m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons + 1] = iTC;
				pVertexIndex[iNumPolygons + 2] = m_iCenter;

				pVertexIndex[iNumPolygons + 3] = m_iCenter;
				pVertexIndex[iNumPolygons + 4] = iTC;
				pVertexIndex[iNumPolygons + 5]= m_iCorner[(_uint)CORNER::CORNER_RT];
				iNumPolygons+=6;
			}
			else
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_LT];
				pVertexIndex[iNumPolygons + 1] = m_iCorner[(_uint)CORNER::CORNER_RT];
				pVertexIndex[iNumPolygons + 2] = m_iCenter;
				iNumPolygons += 3;;
			}

			if (false == isLOD[(_uint)NEIGHBOR::NEIGHBOR_RIGHT])
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_RT];
				pVertexIndex[iNumPolygons + 1] = iRC;
				pVertexIndex[iNumPolygons + 2] = m_iCenter;

				pVertexIndex[iNumPolygons + 3] = m_iCenter;
				pVertexIndex[iNumPolygons + 4] = iRC;
				pVertexIndex[iNumPolygons + 5] = m_iCorner[(_uint)CORNER::CORNER_RB];
				iNumPolygons += 6;

			}
			else
			{
				pVertexIndex[iNumPolygons] = m_iCorner[(_uint)CORNER::CORNER_RT];
				pVertexIndex[iNumPolygons + 1] = m_iCorner[(_uint)CORNER::CORNER_RB];
				pVertexIndex[iNumPolygons + 2] = m_iCenter;
				iNumPolygons+=3;
			}
		}

		return;
	}

	if (true == pFrustum->Isin_Frustum(pLocalPlane, &m_pVerticesPos[m_iCenter], m_fRadius))
	{
		for (size_t i = 0; i < (_uint)CHILD::CHILD_END; i++)
		{
			m_pChild[i]->Culling_ToQuadTree(pFrustum, pLocalPlane, pVertexIndex, iNumPolygons);
		}
	}
}

_bool CQuadTree::Check_LOD()
{
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	matView = matView.Invert();


	_vec3			vCamPosition;
	memcpy(&vCamPosition, &matView.m[3][0], sizeof(_vec3));

	_vec3	vTempDist = vCamPosition - m_pVerticesPos[m_iCenter];
	_vec3	vTempWidth = m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_RT]] - m_pVerticesPos[m_iCorner[(_uint)CORNER::CORNER_LT]];
	_float	fDistance = vTempDist.Length();

	_float	fWidth = vTempWidth.Length();

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

HRESULT CQuadTree::SetUp_Neighbor()
{
	if (nullptr == m_pChild[(_uint)CHILD::CHILD_LT])
		return NOERROR;

	m_pChild[(_uint)CHILD::CHILD_LT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT] = m_pChild[(_uint)CHILD::CHILD_RT];
	m_pChild[(_uint)CHILD::CHILD_LT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM] = m_pChild[(_uint)CHILD::CHILD_LB];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT])
		m_pChild[(_uint)CHILD::CHILD_LT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT]->m_pChild[(_uint)CHILD::CHILD_RT];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP])
		m_pChild[(_uint)CHILD::CHILD_LT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP]->m_pChild[(_uint)CHILD::CHILD_LB];

	m_pChild[(_uint)CHILD::CHILD_RT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT] = m_pChild[(_uint)CHILD::CHILD_LT];
	m_pChild[(_uint)CHILD::CHILD_RT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM] = m_pChild[(_uint)CHILD::CHILD_RB];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT])
		m_pChild[(_uint)CHILD::CHILD_RT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT]->m_pChild[(_uint)CHILD::CHILD_LT];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP])
		m_pChild[(_uint)CHILD::CHILD_RT]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP]->m_pChild[(_uint)CHILD::CHILD_RB];


	m_pChild[(_uint)CHILD::CHILD_RB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT] = m_pChild[(_uint)CHILD::CHILD_LB];
	m_pChild[(_uint)CHILD::CHILD_RB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP] = m_pChild[(_uint)CHILD::CHILD_RT];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT])
		m_pChild[(_uint)CHILD::CHILD_RB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT]->m_pChild[(_uint)CHILD::CHILD_LB];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM])
		m_pChild[(_uint)CHILD::CHILD_RB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM]->m_pChild[(_uint)CHILD::CHILD_RT];

	m_pChild[(_uint)CHILD::CHILD_LB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_RIGHT] = m_pChild[(_uint)CHILD::CHILD_RB];
	m_pChild[(_uint)CHILD::CHILD_LB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_TOP] = m_pChild[(_uint)CHILD::CHILD_LT];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT])
		m_pChild[(_uint)CHILD::CHILD_LB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_LEFT]->m_pChild[(_uint)CHILD::CHILD_RB];
	if (nullptr != m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM])
		m_pChild[(_uint)CHILD::CHILD_LB]->m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM] = m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_BOTTOM]->m_pChild[(_uint)CHILD::CHILD_LT];

	for (size_t i = 0; i < (_uint)CHILD::CHILD_END; i++)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->SetUp_Neighbor();
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
