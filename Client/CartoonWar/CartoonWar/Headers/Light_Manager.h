#pragma once
#include "Base.h"
class CLight;
class CLight_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CLight_Manager)
private:
    CLight_Manager();
    ~CLight_Manager() = default;
public:
    LIGHT        GetLight(const _tchar* pLightTag);
public:
    HRESULT             Add_LightInfo(ID3D12Device* pGraphic_Device, const _tchar* pLightTag,  LIGHT& tLightInfo);
private:
    map<const _tchar*, CLight*>   m_mapLightInfo;
private:
    virtual void        Free();
};

