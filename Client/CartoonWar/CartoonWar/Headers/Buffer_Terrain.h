#pragma once
#include "VIBuffer.h"
class CBuffer_Terrain :
    public CVIBuffer
{
private:
	CBuffer_Terrain();
	CBuffer_Terrain(const CBuffer_Terrain& rhs);
	virtual ~CBuffer_Terrain() = default;
public:
	HRESULT							Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval);
public:
	static	CBuffer_Terrain*		Create(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval=1.f);
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();
private:
	_uint							m_iNumVerticesX = 0.f;
	_uint							m_iNumVerticesZ = 0.f;
	_float							m_fInterval = 0.f;
};

