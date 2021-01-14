#pragma once
#include "Base.h"
class CManagment;
class CServer_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CServer_Manager);
private:
    CServer_Manager();
    virtual ~CServer_Manager() = default;
public:
    void ClientServer(CManagment* managment);
private:
    virtual void Free();
};

