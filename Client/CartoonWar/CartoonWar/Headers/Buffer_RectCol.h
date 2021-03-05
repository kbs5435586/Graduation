#pragma once
#include "VIBuffer.h"
class CBuffer_RectCol :
    public CVIBuffer
{
private:
    CBuffer_RectCol();
    CBuffer_RectCol(const CBuffer_RectCol& rhs);
    virtual ~CBuffer_RectCol() = default;
public:
    HRESULT                 Ready_VIBuffer();
public:
    static CBuffer_RectCol* Create();
    virtual CComponent*     Clone_Component(void* pArg = nullptr);
private:
    virtual void            Free();
};

