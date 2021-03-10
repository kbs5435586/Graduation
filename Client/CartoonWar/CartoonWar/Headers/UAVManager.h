#pragma once
#include "Base.h"
class CUAV;
class CUAVManager :
    public CBase
{
    _DECLARE_SINGLETON(CUAVManager)
private:
    CUAVManager();
    virtual ~CUAVManager() = default;
public:
    HRESULT                     Ready_UAVManager();
private:
    map<const _tchar*, CUAV*>   m_mapUAV;
public:
    CUAV*                       GetUAV(const _tchar* pTag);
    HRESULT                     SetUp_Shader(const _tchar* pTag);
public:
    static CUAVManager*         Create();
private:
    virtual void                Free();
};

/*
필요한거: map first, SetData에 쓸 param, UAVregi, 사용할 쉐이더 이름




*/