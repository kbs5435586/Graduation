#pragma once
#include "VIBuffer.h"
class CBuffer_RectTex :
    public CVIBuffer
{
private:
    CBuffer_RectTex();
    CBuffer_RectTex(const CBuffer_RectTex& rhs);
    virtual ~CBuffer_RectTex() = default;
public:
    HRESULT                 Ready_VIBuffer();
public:
    static CBuffer_RectTex*  Create();
    virtual CComponent*     Clone_Component(void* pArg = nullptr);
private:
    virtual void            Free();
};

