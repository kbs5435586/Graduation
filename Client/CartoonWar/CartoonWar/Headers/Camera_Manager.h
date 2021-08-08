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
    const _matrix           GetIMatView() { return m_ImatView; }
    const _matrix           GetIMatProj() { return m_ImatProj; }
    const _matrix           GetIMatOrtho() { return m_ImatOrtho; }
public:
    const _matrix           GetMapMatView() { return m_MapmatView; }
    const _matrix           GetMapMatProj() { return m_MapmatProj; }
    const _matrix           GetMapMatOrtho() { return m_MapmatOrtho; }
public:
    const _matrix           GetReflectionView() { return m_matReflectionView; }
    const _matrix           GetReflectionMatProj() { return m_matReflectionProj; }
public:
    void                    SetMatView(_matrix matView) { m_matView = matView; }
    void                    SetMatProj(_matrix matProj) { m_matProj = matProj; }
    void                    SetMatOrtho(_matrix matOrtho) { m_matOrtho = matOrtho; }
public:
    void                    SetShadowMatView(_matrix matView) { m_matShadowView = matView; }
    void                    SetShadowMatProj(_matrix matProj) { m_matShadowProj = matProj; }
    void                    SetShadowMatWorld(_matrix matWorld) { m_matShadowWorld = matWorld; }
public:
    void                    SetIMatView(_matrix matView) { m_ImatView = matView; }
    void                    SetIMatProj(_matrix matProj) { m_ImatProj = matProj; }
    void                    SetIMatOrtho(_matrix matOrtho) { m_ImatOrtho = matOrtho; }
public:
    void                    SetMapMatView(_matrix matView) { m_MapmatView = matView; }
    void                    SetMapMatProj(_matrix matProj) { m_MapmatProj = matProj; }
    void                    SetMapMatOrtho(_matrix matOrtho) { m_MapmatOrtho = matOrtho; }
public:
    void                    SetReflectionView(_matrix matView) { m_matReflectionView = matView; }
    void                    SetReflectionProj(_matrix matProj) { m_matReflectionProj = matProj; }
private:
    _matrix                 m_matReflectionView = {};
    _matrix                 m_matReflectionProj = {};
private:
    _matrix                 m_matView = {};
    _matrix                 m_matProj = {};
    _matrix                 m_matOrtho = {};
private:
    _matrix                 m_matShadowWorld = {};
    _matrix                 m_matShadowView = {};
    _matrix                 m_matShadowProj = {};
private:
    _matrix                 m_ImatView = {};
    _matrix                 m_ImatProj = {};
    _matrix                 m_ImatOrtho = {};
private:
    _matrix                 m_MapmatView = {};
    _matrix                 m_MapmatProj = {};
    _matrix                 m_MapmatOrtho = {};
private:
    virtual void			Free();
};

