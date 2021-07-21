#pragma once
#include "VIBuffer.h"
class CBuffer_CubeNor :
	public CVIBuffer
{
private:
	CBuffer_CubeNor();
	CBuffer_CubeNor(const CBuffer_CubeNor& rhs);
	virtual ~CBuffer_CubeNor() = default;
public:
	HRESULT						Ready_VIBuffer();
public:
	static	CBuffer_CubeNor* Create();
	virtual CComponent* Clone_Component(void* pArg = nullptr);
protected:
	virtual void				Free();
};

