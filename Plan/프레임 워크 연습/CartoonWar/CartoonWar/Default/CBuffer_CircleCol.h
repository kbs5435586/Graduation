#pragma once
#include "VIBuffer.h"
class CBuffer_CircleCol :
    public CVIBuffer
{
    CBuffer_CircleCol();
    CBuffer_CircleCol(const CBuffer_CircleCol& rhs);
    virtual ~CBuffer_CircleCol() = default;

public:
    HRESULT                     Ready_VIBuffer();

public:
    static  CBuffer_CircleCol*  Create();
    virtual CComponent*         Clone_Component(void* pArg = nullptr);

private:
    virtual void                Free();

};

