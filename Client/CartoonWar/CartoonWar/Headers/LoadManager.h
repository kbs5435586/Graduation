#pragma once
#include "Base.h"
class CLoadManager :
    public CBase
{
   _DECLARE_SINGLETON(CLoadManager)
private:
    CLoadManager();
    virtual ~CLoadManager() = default;
public:
    HRESULT                 Ready_LoadManager();
    HRESULT                 Load_File(const _tchar* pFilePath, void* pArg);
public: 
    static CLoadManager*    Create();
private:
    virtual void            Free();
};

