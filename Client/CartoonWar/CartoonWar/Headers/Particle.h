#pragma once
#include "Component.h"
class CStructedBuffer;
class CParticle :
    public CComponent
{
private:
    CParticle();
    CParticle(const CParticle& rhs);
    virtual ~CParticle() = default;
public:
    HRESULT                     Ready_Particle();
    void                        Update_Particle(const _float& fTimeDelta);
    void                        Render_Particle();
    HRESULT                     SetUp_OnShader(REP& tRep0, REP& tRep1);
private:
    CStructedBuffer*            m_pParticleBuffer = nullptr;
    CStructedBuffer*            m_pShadedBuffer = nullptr;
public:
    const _uint&                GetMaxParticle(){return m_iMaxParticle;}
private:
    _uint                       m_iMaxParticle = 1000;
    _float                      m_fFrequency = 0.002f;
    _float                      m_fAccTime = 0.f;
    _float                      m_fMinLifeTime = 0.5f;
    _float                      m_fMaxLifeTime = 1.f;
    _float                      m_fMinSpeed = 100;
    _float                      m_fMaxSpeed = 50.f;
    _float                      m_fStartScale = 30.f;
    _float                      m_fEndScale = 10.f;
private:
    _vec4                       m_vStartColor= _vec4(0.2f, 0.2f, 0.8f, 1.4f);
    _vec4                       m_vEndColor= _vec4(0.2f, 0.2f, 0.8f, 1.4f);
private:
    _uint                       m_iAddCnt = 0;
public:
    static CParticle*           Create();
    virtual CComponent*         Clone_Component(void* pArg = nullptr);
protected:
    virtual void				Free();


};

