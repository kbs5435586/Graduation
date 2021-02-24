#pragma once
#include "VIBuffer.h"
class CBuffer_Sphere :
    public CVIBuffer
{
private:
    CBuffer_Sphere();
    CBuffer_Sphere(const CBuffer_Sphere& rhs);
    virtual ~CBuffer_Sphere() = default;
public:
	HRESULT							Ready_VIBuffer();

public:
	static	CBuffer_Sphere*			Create();
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();

};

