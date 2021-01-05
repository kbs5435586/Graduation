#pragma once
#include "Component.h"
#include "Picking.h"
#include "Transform.h"
class CVIBuffer :
	public CComponent
{
protected:
	CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	HRESULT Ready_VIBuffer();
	virtual _bool		Picking_ToBuffer(_vec3* pOut, const CTransform* pTransformCOm, const CPicking* pPickingCom) { return true; }
protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	_uint							m_iStride = 0; // 정점 하나의 크기.
	_uint							m_iNumVertices; // 정점들의 갯수.
	_ulong							m_dwFVF = 0; // 정점 멤버를 구성하는 fvf열거체.
protected:
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;
	_uint							m_iPolygonSize = 0;
	_uint							m_iNumPolygons = 0;
	D3DFORMAT						m_eFormat = D3DFORMAT(0);
protected:
	_vec3*							m_pPosition = nullptr;
public:
	virtual CComponent* Clone_Component(void* pArg) = 0;
	virtual void Free();
};

