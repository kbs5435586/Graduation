#pragma once
#include "Base.h"
class CFrustum;
class CQuadTree :
    public CBase
{
public:
	enum class NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
	enum class CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum class CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
private:
	CQuadTree();
	virtual ~CQuadTree() = default;
public:
	HRESULT				Ready_QuadTree(_vec3 * pVerticesPos, const _uint & iNumVerticeX, const _uint & iNumVerticeZ);
	HRESULT				Ready_QuadTree(_vec3 * pVerticesPos, const _uint & iLT, const _uint & iRT, const _uint & iRB, const _uint & iLB);
	void				Culling_ToQuadTree(CFrustum * pFrustum, Plane* pLocalPlane, const _uint& pVertexIndex, _uint & iNumPolygons);
	_bool				Check_LOD();
private:
	CQuadTree*			m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_END] = { nullptr };
	CQuadTree*			m_pChild[(_uint)CHILD::CHILD_END] = { nullptr };
	_uint				m_iCorner[(_uint)CORNER::CORNER_END] = { 0 }; // 현재 쿼드트리(노드)가 대표하는 사각형 지형의 네개 꼭지점. 
	_uint				m_iCenter = 0;
	_float				m_fRadius = 0.f;
	_vec3*				m_pVerticesPos = nullptr;
private:
	HRESULT				SetUp_ChildNode();
public:
	static CQuadTree*	Create(_vec3 * pVerticesPos, const _uint & iNumVerticeX, const _uint & iNumVerticeZ);
	static CQuadTree*	Create(_vec3 * pVerticesPos, const _uint & iLT, const _uint & iRT, const _uint & iRB, const _uint & iLB);
private:
	virtual void		Free();
};									

