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
    CMRT*                                   Get_RTT(const _uint& iIdx) { return m_vecMRT[iIdx]; }
    CRTT*                                   GetPostEffectTex(){return m_pPostEffectTex;}
    CRTT*                                   GetBlurTex(){return m_pBlurTex;}
public:
    static CRTTMananger*                    Create();
    virtual void                            Free();
private:
    vector<CMRT*>                           m_vecMRT;
    _uint                                   m_iSize = 0;
    CRTT*                                   m_pDsTex = nullptr;
    CRTT*                                   m_pDsInvenTex = nullptr;
    CRTT*                                   m_pDsMapTex = nullptr;

    CRTT*                                   m_pPostEffectTex = nullptr;
    CRTT*                                   m_pBlurTex = nullptr;
};

