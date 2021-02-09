#pragma once
#include "Base.h"
#include "RTT.h"

struct tRtt
{
    CRTT* pRtt;
    _vec4   vClear_Color;
};
class CMRT :
    public CBase
{
private:
    CMRT();
    virtual ~CMRT() = default;
public:
    HRESULT                         Ready_MRT(_uint iCnt, tRtt* arrRT, CRTT* pDsTex);
    void                            OM_Set(_uint iCnt, _uint iOffset = 0);
    void                            OM_Set();
    void                            Clear();
    void                            Clear(_uint iRtIdx);
public:
    static CMRT*                    Create(_uint iCnt, tRtt* arrRT, CRTT* pDsTex);
private:
    virtual void                    Free();
private:
    tRtt                            m_tArr[8] = {};
    _uint                           m_iRTCnt = 0;
    CRTT*                           m_pDsTex= nullptr;
    ComPtr<ID3D12DescriptorHeap>    m_pRTV;
};

