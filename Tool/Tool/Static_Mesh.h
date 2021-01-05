#pragma once
#include "Component.h"
class CPicking;
class CTransform;
class CStatic_Mesh :
	public CComponent
{
private:
	CStatic_Mesh(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;
public:
	HRESULT						Ready_Mesh_Static(const _tchar* pFilePath, const _tchar* pFileName);
	void						Render_Mesh(_uint iAttributeID);
public:
	const SUBSETDESC*			Get_SubSetDesc(const _uint& iAttributeID) const {	return &m_pSubSetDesc[iAttributeID];	}
	const _ulong				Get_NumMaterials() const {	return m_dwNumMaterials;	}
public:
	static CStatic_Mesh*		Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent*			Clone_Component(void* pArg);
	virtual void				Free();
private:
	HRESULT						Change_TextureFileName(_tchar* pFullPath, const _tchar* pSour, const _tchar* pDest);
public:
	_bool						Picking_ToMesh(_vec3* pOut, const CTransform* pTransformCOm, const CPicking* pPickingCom);

private:
	LPD3DXMESH					m_pMesh = nullptr;
	LPD3DXBUFFER				m_pAdjacency = nullptr;
	LPD3DXBUFFER				m_pMaterials = nullptr;
	_ulong						m_dwNumMaterials = 0;
	SUBSETDESC*					m_pSubSetDesc = nullptr;
private:
	_vec3						m_vMin, m_vMax;
	_vec3*						m_pPosition = nullptr;
	_vec3						m_vTempMin, m_vTempMax;


};

