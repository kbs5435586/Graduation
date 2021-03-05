#pragma once
#include "VIBuffer.h"
class CBuffer_CubeTex :
    public CVIBuffer
{
private:
    CBuffer_CubeTex();
    CBuffer_CubeTex(const CBuffer_CubeTex& rhs);
    virtual ~CBuffer_CubeTex() = default;
public:
	HRESULT							Ready_VIBuffer();
public:
	static	CBuffer_CubeTex*		Create();
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();
};

