#pragma once
#include "VIBuffer.h"
class CBuffer_Terrain :
    public CVIBuffer
{
private:
    CBuffer_Terrain(ID3D12Device* pGraphic_Device);
    CBuffer_Terrain(const CBuffer_Terrain& rhs);
    virtual ~CBuffer_Terrain() = default;
public:
	HRESULT					Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval);
	void					Render_VIBuffer();
public:
	static					CBuffer_Terrain* Create(ID3D12Device* pGraphic_Device, const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval=1.f);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
private:
	_uint					m_iNumVerticesX = 0;
	_uint					m_iNumVerticesZ = 0;
	_float					m_fInterval = 0.f;
private:
	VTXCOL*					m_pVertices = nullptr;
	_uint*					m_pIndices = nullptr;
};

