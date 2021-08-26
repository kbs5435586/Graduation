#pragma once
#include "VIBuffer.h"
class CBuffer_Cone :
    public CVIBuffer
{
private:
    CBuffer_Cone();
    CBuffer_Cone(const CBuffer_Cone& rhs);
    virtual ~CBuffer_Cone() = default;

public:
    HRESULT                     Ready_VIBuffer();

public:
    static  CBuffer_Cone* Create();
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


