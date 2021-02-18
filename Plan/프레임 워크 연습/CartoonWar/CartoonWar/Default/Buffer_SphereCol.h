#pragma once
#include "VIBuffer.h"
class CBuffer_SphereCol :
    public CVIBuffer
{
public:
    CBuffer_SphereCol();
    CBuffer_SphereCol(const CBuffer_SphereCol& rhs);
    virtual ~CBuffer_SphereCol() = default;

public:
    HRESULT                     Ready_VIBuffer();

public:
    static  CBuffer_SphereCol* Create();
    virtual CComponent* Clone_Component(void* pArg = nullptr);

private:
    virtual void                Free();

};

