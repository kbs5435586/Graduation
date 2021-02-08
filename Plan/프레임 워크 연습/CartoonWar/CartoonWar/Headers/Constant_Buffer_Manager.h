#pragma once
#include "Base.h"
#include "Constant_Buffer.h"
class CConstant_Buffer_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CConstant_Buffer_Manager)
private:
    CConstant_Buffer_Manager();
    virtual ~CConstant_Buffer_Manager() = default;
public:
    HRESULT                                 Create_Constant_Buffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType, _bool IsGlobal = false);
public:
    CConstant_Buffer*                       GetConstantBuffer(_uint iIdx) { return m_vecConstantBuffer[iIdx]; }
    vector<CConstant_Buffer*>               GetConstantBuffer() { return m_vecConstantBuffer; }
private:
    vector<CConstant_Buffer*>               m_vecConstantBuffer;
private:
    virtual void                            Free();
};

