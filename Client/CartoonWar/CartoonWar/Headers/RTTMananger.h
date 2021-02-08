#pragma once
#include "Base.h"
#include "RTT.h"
class CRTTMananger :
    public CBase
{
    _DECLARE_SINGLETON(CRTTMananger)
private:
    CRTTMananger();
    virtual ~CRTTMananger() = default;
public:
    HRESULT                                 Ready_RTTMananger(const _tchar* pRTT_Tag);
    void                                    Set_RenderTarget(const _tchar* pRTT_Tag, ID3D12DescriptorHeap* pDsv);
    CRTT*                                   Get_RTT(const _tchar* pRTT_Tag);
public:
    static CRTTMananger*                    Create(const _tchar* pRTT_Tag, _uint iTextureWidth, _uint iTextureHeight);
public:
    CRTT*                                     Create_TextureFromResource(const _tchar* pRTT_Tag, ComPtr<ID3D12Resource> pResource);
private:
    virtual void                            Free();
private:
    map<const _tchar*, vector<CRTT*>>       m_mapRTT;
    _uint                                   m_iSize = 0;

};

