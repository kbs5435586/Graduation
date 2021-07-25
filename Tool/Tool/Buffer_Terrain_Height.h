#pragma once
#include "VIBuffer.h"
class CBuffer_Terrain_Height :
	public CVIBuffer
{
private:
	explicit CBuffer_Terrain_Height(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_Terrain_Height(const CBuffer_Terrain_Height& rhs);
	virtual ~CBuffer_Terrain_Height() = default;
public:
	HRESULT Ready_VIBuffer(const _tchar* pHeightMap, const _float& fInterval);
	void Render_VIBuffer();
private:
	_uint				m_iNumVerticesX = 0;
	_uint				m_iNumVerticesZ = 0;
	_float				m_fInterval = 0.f;
	BITMAPFILEHEADER	m_fh;
	BITMAPINFOHEADER	m_ih;
	_ulong*				m_pPixel = nullptr;
private:
	POLYGON32*			m_pPolygonVertexIndex = nullptr;

public:
	static CBuffer_Terrain_Height* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pHeightMap, const _float& fInterval = 1.f);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
protected:
	virtual void Free();
};

