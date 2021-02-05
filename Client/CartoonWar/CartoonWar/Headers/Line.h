#pragma once
#include "Base.h"
class CLine :
    public CBase
{
private:
    CLine();
    virtual ~CLine() = default;
private:
    HRESULT         Ready_Line(const _vec3& vStart, const _vec3 vEnd);
public:
    _bool           IsRight(const _vec3& vPos);
private:
    _vec3           m_vStart;
    _vec3           m_vEnd;
    _vec3           m_vNormal;
public:
    static CLine*   Create(const _vec3& vStart, const _vec3 vEnd);
private:
    virtual void    Free();
};

