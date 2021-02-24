#pragma once
#include "VIBuffer.h"
class CBuffer_UITex :
    public CVIBuffer
{
private:
	CBuffer_UITex();
	CBuffer_UITex(const CBuffer_UITex& rhs);
	virtual ~CBuffer_UITex() = default;
public:
	HRESULT						Ready_VIBuffer();
public:
	static CBuffer_UITex*		Create();
	virtual CComponent*			Clone_Component(void* pArg = nullptr) ;
protected:
	virtual void				Free();
};

