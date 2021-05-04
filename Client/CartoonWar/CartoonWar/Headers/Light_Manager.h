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
    LIGHT                       GetLight(_uint& iIdx);
    HRESULT                     Add_LightInfo(LIGHT& tLightInfo);
    void                        SetUp_OnShader();
public:
    void                        Update();
    void                        Render();
private:
    vector<CLight*>             m_vecLightInfo;
private:
    virtual void                Free();
};

