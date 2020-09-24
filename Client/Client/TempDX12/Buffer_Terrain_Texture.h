#pragma once
#include "VIBuffer.h"
class CBuffer_Terrain_Texture :
    public CVIBuffer
{
private:
	CBuffer_Terrain_Texture(ID3D12Device* pGraphic_Device);
	CBuffer_Terrain_Texture(const CBuffer_Terrain_Texture& rhs);
	virtual ~CBuffer_Terrain_Texture() = default;
public:
	HRESULT					Ready_VIBuffer(const _uint & iNumVerticesX, const _uint & iNumVerticesZ, const _float & fInterval);
	void					Render_VIBuffer();
public:
	static					CBuffer_Terrain_Texture* Create(ID3D12Device * pGraphic_Device, const _uint & iNumVerticesX, const _uint & iNumVerticesZ, const _float & fInterval = 1.f);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
private:
	_uint					m_iNumVerticesX = 0;
	_uint					m_iNumVerticesZ = 0;
	_float					m_fInterval = 0.f;
private:
	VTXTEX*					m_pVertices = nullptr;
	_uint*					m_pIndices = nullptr;
};

