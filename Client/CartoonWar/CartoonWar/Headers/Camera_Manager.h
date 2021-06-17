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
    const _matrix           GetMatOrtho() { return m_matOrtho; }
public:
    const _matrix           GetShadowView() { return m_matShadowView; }
    const _matrix           GetShadowMatProj() { return m_matShadowProj; }
    const _matrix           GetShadowWorld() { return m_matShadowWorld; }
public:
    void                    SetMatView(_matrix matView) { m_matView = matView; }
    void                    SetMatProj(_matrix matProj) { m_matProj = matProj; }
    void                    SetMatOrtho(_matrix matOrtho) { m_matOrtho = matOrtho; }
public:
    void                    SetShadowMatView(_matrix matView) { m_matShadowView = matView; }
    void                    SetShadowMatProj(_matrix matProj) { m_matShadowProj = matProj; }
    void                    SetShadowMatWorld(_matrix matWorld) { m_matShadowWorld = matWorld; }
private:
    _matrix                 m_matView = {};
    _matrix                 m_matProj = {};
    _matrix                 m_matOrtho = {};
private:
    _matrix                 m_matShadowWorld = {};
    _matrix                 m_matShadowView = {};
    _matrix                 m_matShadowProj = {};
private:
    virtual void			Free();
};

