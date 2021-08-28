#pragma once
#include "VIBuffer.h"
class CBuffer_Cylinder :
    public CVIBuffer
{
private:
    CBuffer_Cylinder();
    CBuffer_Cylinder(const CBuffer_Cylinder& rhs);
    virtual ~CBuffer_Cylinder() = default;

public:
    HRESULT                     Ready_VIBuffer();

public:
    static  CBuffer_Cylinder* Create();
    virtual CComponent* Clone_Component(void* pArg = nullptr);

private:
    virtual void                Free();

private:
    _float topRadius{};
    _float botRadius{};

    _float height{};

    _uint sliceCount{};
    _uint stackCount{};

private:
    void BuildTopCap(vector<VTXTEXCUBE>& vertices, vector<_uint>& indices);
    void BuildBotCap(vector<VTXTEXCUBE>& vertices, vector<_uint>& indices);

};


