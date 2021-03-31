#pragma once
#include "VIBuffer.h"
class CBuffer_Point :
    public CVIBuffer
{
private:
    CBuffer_Point();
	CBuffer_Point(const CBuffer_Point& rhs);
	virtual ~CBuffer_Point() = default;
public:
	HRESULT							Ready_VIBuffer();

public:
	static	CBuffer_Point*			Create();
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();
};

