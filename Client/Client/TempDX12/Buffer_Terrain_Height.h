#pragma once
#include "VIBuffer.h"
class CTransform;
class CBuffer_Terrain_Height :
    public CVIBuffer
{
private:
	CBuffer_Terrain_Height(ID3D12Device* pGraphic_Device);
	CBuffer_Terrain_Height(const CBuffer_Terrain_Height& rhs);
	virtual ~CBuffer_Terrain_Height() = default;
public:
	HRESULT					Ready_VIBuffer(const _tchar* pFilePath, const _float& fInterval);
	void					Render_VIBuffer();
public:
	static					CBuffer_Terrain_Height* Create(ID3D12Device* pGraphic_Device, const _tchar* pFilePath, const _float& fInterval = 1.f);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
public:
	_float									Compute_HeightOnTerrain(CTransform* pTransform);
protected:
	virtual void			Free();
private:
	_uint					m_iNumVerticesX = 0;
	_uint					m_iNumVerticesZ = 0;
	_float					m_fInterval = 0.f;
private:
	VTXTEXNOR* m_pVertices = nullptr;
	_uint* m_pIndices = nullptr;
private:
	BITMAPFILEHEADER					m_fh;
	BITMAPINFOHEADER					m_ih;
	_vec3* m_pPosition = nullptr;
	_ulong* m_pPixel = nullptr;
	_uint								m_iNumPolygons = 0;
};

