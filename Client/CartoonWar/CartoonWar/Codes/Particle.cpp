#include "framework.h"
#include "Particle.h"
#include "StructedBuffer.h"

CParticle::CParticle()
	: CComponent()
{
}

CParticle::CParticle(const CParticle& rhs)
	: CComponent(rhs)
	, m_pParticleBuffer(rhs.m_pParticleBuffer)
	, m_pShadedBuffer(rhs.m_pShadedBuffer)
	, m_iMaxParticle(rhs.m_iMaxParticle)
	, m_fFrequency(rhs.m_fFrequency)
	, m_fAccTime(rhs.m_fAccTime)
	, m_fMinLifeTime(rhs.m_fMinLifeTime)
	, m_fMaxLifeTime(rhs.m_fMaxLifeTime)
	, m_fMinSpeed(rhs.m_fMinSpeed)
	, m_fMaxSpeed(rhs.m_fMaxSpeed)
	, m_fStartScale(rhs.m_fStartScale)
	, m_fEndScale(rhs.m_fEndScale)
	, m_vStartColor(rhs.m_vStartColor)
	, m_vEndColor(rhs.m_vEndColor)
{

}

HRESULT CParticle::Ready_Particle()
{
	m_pParticleBuffer = CStructedBuffer::Create(sizeof(PARTICLE), m_iMaxParticle);
	if (nullptr == m_pParticleBuffer)
		return E_FAIL;
	m_pShadedBuffer = CStructedBuffer::Create(sizeof(PARTICLESHARED), 1);
	if (nullptr == m_pShadedBuffer)
		return E_FAIL;

	
	return S_OK;
}

void CParticle::Update_Particle(const _float& fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	m_iAddCnt = 0;

	if (m_fFrequency < m_fAccTime)
	{
		m_fAccTime -= m_fFrequency;
		m_iAddCnt = 1;
	}

	m_pParticleBuffer->Update_RWData(UAV_REGISTER::u0);
	m_pShadedBuffer->Update_RWData(UAV_REGISTER::u1);

}

void CParticle::Render_Particle()
{
}

HRESULT CParticle::SetUp_OnShader(REP& tRep0, REP& tRep1)
{
	tRep0.m_arrVec4[0]	= m_vStartColor;
	tRep0.m_arrVec4[1]	= m_vEndColor;

	tRep0.m_arrFloat[0] = m_fStartScale;
	tRep0.m_arrFloat[1] = m_fEndScale;


	tRep1.m_arrInt[0]	= m_iMaxParticle;
	tRep1.m_arrInt[1]	= m_iAddCnt;
	tRep1.m_arrFloat[0] = m_fMinLifeTime;
	tRep1.m_arrFloat[1] = m_fMinLifeTime;
	tRep1.m_arrFloat[2] = m_fMinSpeed;
	tRep1.m_arrFloat[3] = m_fMaxSpeed;

	return S_OK;
}

CParticle* CParticle::Create()
{
	CParticle* pInstnace = new CParticle;
	if (FAILED(pInstnace->Ready_Component()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CComponent* CParticle::Clone_Component(void* pArg)
{
	return new CParticle (*this);
}

void CParticle::Free()
{
	Safe_Release(m_pParticleBuffer);
	Safe_Release(m_pShadedBuffer);
	CComponent::Free();
}