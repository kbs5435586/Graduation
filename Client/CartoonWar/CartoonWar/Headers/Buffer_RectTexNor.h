#pragma once
#include "VIBuffer.h"
class CBuffer_RectTexNor :
    public CVIBuffer
{
private:
    CBuffer_RectTexNor();
    CBuffer_RectTexNor(const CBuffer_RectTexNor& rhs);
    virtual ~CBuffer_RectTexNor() = default;
public:
    HRESULT                 Ready_VIBuffer();
public:
    static CBuffer_RectTexNor* Create();
    virtual CComponent* Clone_Component(void* pArg = nullptr);
private:
    virtual void            Free();
};

