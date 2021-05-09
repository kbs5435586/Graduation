#pragma once
#include "VIBuffer.h"
class CBuffer_Circle :
    public CVIBuffer
{
private:
    CBuffer_Circle();
    CBuffer_Circle(const CBuffer_Circle& rhs);
    virtual ~CBuffer_Circle() = default;

public:
    HRESULT                     Ready_VIBuffer();

public:
    static  CBuffer_Circle* Create();
    virtual CComponent* Clone_Component(void* pArg = nullptr);

private:
    virtual void                Free();
};

