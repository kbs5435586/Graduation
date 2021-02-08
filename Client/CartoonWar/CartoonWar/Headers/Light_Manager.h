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
    LIGHT*                      GetLight(const _tchar* pLightTag);
    HRESULT                     Add_LightInfo(const _tchar* pLightTag,  LIGHT& tLightInfo);
public:
    void                        SetUp_OnShader();
private:
    map<const _tchar*, CLight*> m_mapLightInfo;
private:
    virtual void        Free();
};

