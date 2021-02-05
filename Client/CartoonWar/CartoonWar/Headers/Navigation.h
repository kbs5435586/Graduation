#pragma once
#include "Component.h"
class CCell;
class CNavigation :
    public CComponent
{
private:
    CNavigation();
    CNavigation(const CNavigation& rhs);
    virtual ~CNavigation() = default;
public:
    HRESULT                 Ready_Navigation(const _tchar* pFilePath);
    HRESULT                 Ready_Neighbor();
    HRESULT                 Ready_Clone_Navigation(void* pArg);
    void                    Render_Navigation();
    _bool                   Move_OnNavigation(const _vec3* vPos, const _vec3* vDirectionPerSec);

private:
    vector<CCell*>          m_vecCell;
    vector<_vec3>           m_vecPos;
    vector<_vec3>           m_vecTotalPos;
private:
    _uint                   m_iCurrentIdx = 0;
public:
    static CNavigation*     Create(const _tchar* pFilePath);
    virtual CComponent*     Clone_Component(void* pArg);
private:
    virtual void            Free();
};

