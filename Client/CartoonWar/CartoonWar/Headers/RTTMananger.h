#pragma once
#include "Base.h"

class CMRT;
class CRTT;

class CRTTMananger :
    public CBase
{
    _DECLARE_SINGLETON(CRTTMananger)
private:
    CRTTMananger();
    virtual ~CRTTMananger() = default;
public:
    HRESULT                                 Ready_RTTMananger();
    void                                    Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv);
    CRTT*                                   Get_RTT(const _tchar* pRTT_Tag);
public:
    static CRTTMananger*                    Create();
    virtual void                            Free();
private:
    vector<CMRT*>                           m_vecMRT;
    _uint                                   m_iSize = 0;

};

