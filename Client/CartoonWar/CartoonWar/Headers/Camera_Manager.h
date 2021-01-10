#pragma once
#include "Base.h"
class CCamera_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CCamera_Manager);
private:
    CCamera_Manager();
    virtual ~CCamera_Manager() = default;
public:
    const _matrix           GetMatView() { return m_matView; }
    const _matrix           GetMatProj() { return m_matProj; }
public:
    void                    SetMatView(_matrix matView) { m_matView = matView; }
    void                    SetMatProj(_matrix matProj) { m_matProj = matProj; }
private:
    _matrix                 m_matView = {};
    _matrix                 m_matProj = {};
private:
    virtual void			Free();
};

