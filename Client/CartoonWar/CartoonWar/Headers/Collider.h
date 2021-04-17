#pragma once
#include "Component.h"
class CShader;
class CTransform;
class CBuffer_CubeCol;
class CBuffer_Sphere;
class CVIBuffer;

class CCollider :
    public CComponent
{
private:
    CCollider();
    CCollider(const CCollider& rhs);
    virtual ~CCollider() = default;
public:
    HRESULT                     Ready_Collider(COLLIDER_TYPE eType);
    HRESULT                     Ready_Collider_AABB_BOX(CTransform* pTransform, const _vec3 vSize);
    HRESULT                     Ready_Collider_OBB_BOX(CTransform* pTransform, const _vec3 vSize);
    HRESULT                     Ready_Collider_SPHERE(CTransform* pTransform, const _vec3 vSize);
public:
    HRESULT                     Ready_Collider_AABB_BOX(_matrix matWorld, const _vec3 vSize);
    HRESULT                     Ready_Collider_OBB_BOX(_matrix matWorld, const _vec3 vSize);
    HRESULT                     Ready_Collider_SPHERE(_matrix matWorld, const _vec3 vSize);
public:
    HRESULT                     Clone_ColliderBox(CTransform* pTransform, const _vec3 vSize);
    HRESULT                     Clone_ColliderBox(_matrix matWorld, const _vec3 vSize);
public:
    void                        Update_Collider(CTransform* pTransform);
private:
    _matrix                     Remove_Rotation(_matrix matWorld);
    HRESULT                     Create_InputLayOut();
public:
    void                        Render_Collider();
public:
    static CCollider*           Create(COLLIDER_TYPE eType);
    virtual CComponent*         Clone_Component(void* pArg);
private:
    virtual void                Free();
private:
    COLLIDER_TYPE               m_eType = COLLIDER_TYPE::COLLIDER_END;
    _vec3                       m_vSize = {};
    _float                      m_fRadius = 0.f;
private:
    CShader*                    m_pShaderCom = nullptr;
    CTransform*                 m_pTransformCom = nullptr;
    CVIBuffer*                  m_pBufferCom = nullptr;
    
};

