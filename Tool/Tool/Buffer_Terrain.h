#pragma once
#include "VIBuffer.h"
class CBuffer_Terrain :
	public CVIBuffer
{
private:
	CBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBuffer_Terrain(const CBuffer_Terrain& rhs);
	virtual ~CBuffer_Terrain()=default;
public:
	HRESULT					Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval); 
	HRESULT					Ready_VIBuffer(const _tchar* pFilePath, const _float& fInterval);
	void					Render_VIBuffer();
public:
	static CBuffer_Terrain*	Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval = 1.f);
	static CBuffer_Terrain*	Create(LPDIRECT3DDEVICE9 pGrpahic_Device, const _tchar* pFilePath, const _float& fInterval = 1.f);
	virtual CComponent*		Clone_Component(void* pArg);
	virtual void			Free();
public:
	virtual _bool			Picking_ToBuffer(_vec3* pOut, const CTransform* pTransformCOm, const CPicking* pPickingCom);
public:
	_float					Compute_HeightOnTerrain(const CTransform* pTransform);
private:
	_uint					m_iNumVerticesX = 0;
	_uint					m_iNumVerticesZ = 0;
	_float					m_fInterval = 0.f;
private:
	BITMAPFILEHEADER		m_File_Header;
	BITMAPINFOHEADER		m_Info_Header;
	_ulong*					m_pPixel = nullptr;
};

