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
    HRESULT                     Ready_Collider_AABB_BOX(CTransform* pTransform, const _vec3 vSize = { 1.f,1.f,1.f });
    HRESULT                     Ready_Collider_OBB_BOX(CTransform* pTransform, const _vec3 vSize = { 1.f,1.f,1.f });
    HRESULT                     Ready_Collider_SPHERE(CTransform* pTransform, const _vec3 vSize = { 1.f,1.f,1.f });
public:
    HRESULT                     Ready_Collider_AABB_BOX(_matrix matWorld, const _vec3 vSize = { 1.f,1.f,1.f });
    HRESULT                     Ready_Collider_OBB_BOX(_matrix matWorld, const _vec3 vSize = { 1.f,1.f,1.f });
    HRESULT                     Ready_Collider_SPHERE(_matrix matWorld, const _vec3 vSize = { 1.f,1.f,1.f });
public:
    HRESULT                     Clone_ColliderBox(CTransform* pTransform, const _vec3 vSize = { 1.f,1.f,1.f });
    HRESULT                     Clone_ColliderBox(_matrix matWorld, const _vec3 vSize = { 1.f,1.f,1.f });
public:
    void                        Change_ColliderBoxSize(CTransform* pTransform, const _vec3 vSize);
public:
    _bool                       Collision_AABB(CCollider* pTargetCollider);
    void				        Collision_AABB(CCollider* pTargetCollider, CTransform* pSourTransform, CTransform* pDestTransform);
    _bool                       Collision_OBB(CCollider* pTargetCollider);
public:
    void                        Update_Collider(CTransform* pTransform, _vec3 vSize ,CLASS eCurClass = CLASS::CLASS_END);
    void                        Update_Collider_Ex(CTransform* pTransform);
    _matrix				        Compute_WorldTransform();
private:
    void				        Compute_AlignAxis(OBB* pOBB);
    void				        Compute_ProjAxis(OBB* pOBB);
    _matrix                     Remove_Rotation(_matrix matWorld);
    HRESULT                     Create_InputLayOut();
public:
    void                        Render_Collider(_uint iColorTemp = 0);
public:
    static CCollider*           Create(COLLIDER_TYPE eType);
    virtual CComponent*         Clone_Component(void* pArg);
private:
    virtual void                Free();
private:
    COLLIDER_TYPE               m_eType = COLLIDER_TYPE::COLLIDER_END;
    _vec3                       m_vSize = {};
    _float                      m_fRadius = 0.f;
    _bool                       m_IsColl = false;
private:
    OBB*                        m_pOBB = nullptr;
    _vec3                       m_vMin = {};
    _vec3                       m_vMax = {};
    _matrix                     m_matWorld = {};
private:
    CShader*                    m_pShaderCom = nullptr;
    CTransform*                 m_pTransformCom = nullptr;
    CVIBuffer*                  m_pBufferCom = nullptr;
    
};

