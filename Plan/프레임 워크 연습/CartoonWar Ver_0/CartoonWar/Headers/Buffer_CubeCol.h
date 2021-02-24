#pragma once
#include "VIBuffer.h"
class CBuffer_CubeCol :
    public CVIBuffer
{
private:
    CBuffer_CubeCol();
    CBuffer_CubeCol(const CBuffer_CubeCol& rhs);
    virtual ~CBuffer_CubeCol() = default;
public:
	HRESULT						Ready_VIBuffer();
public:
	static	CBuffer_CubeCol*	Create();
	virtual CComponent*			Clone_Component(void* pArg = nullptr);
protected:
	virtual void				Free();
};

