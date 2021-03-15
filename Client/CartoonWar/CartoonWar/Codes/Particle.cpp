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
}

void CParticle::Render_Particle()
{
	m_pParticleBuffer->Update_RWData(UAV_REGISTER::u0);
	m_pShadedBuffer->Update_RWData(UAV_REGISTER::u1);
}

HRESULT CParticle::SetUp_OnUpdateShader(REP& tRep)
{

	tRep.m_arrInt[0] = m_iMaxParticle;
	tRep.m_arrInt[1] = m_iAddCnt;
	tRep.m_arrFloat[0] = m_fMinLifeTime;
	tRep.m_arrFloat[1] = m_fMinLifeTime;
	tRep.m_arrFloat[2] = m_fMinSpeed;
	tRep.m_arrFloat[3] = m_fMaxSpeed;

	return E_NOTIMPL;
}

HRESULT CParticle::SetUp_OnShader(REP& tRep)
{
	tRep.m_arrVec4[0]	= m_vStartColor;
	tRep.m_arrVec4[1]	= m_vEndColor;

	tRep.m_arrFloat[0] = m_fStartScale;
	tRep.m_arrFloat[1] = m_fEndScale;



	m_pParticleBuffer->Update_Data(TEXTURE_REGISTER::t10);
	return S_OK;
}

void CParticle::DisPatch(_int x, _int y, _int z)
{
	CDevice::GetInstance()->UpdateTable_CS();
	CDevice::GetInstance()->GetCsCmdLst()->Dispatch(x, y, z);
	CDevice::GetInstance()->ExcuteComputeShader();
}

void CParticle::Update_Particle_Shader()
{
	m_pParticleBuffer->Update_RWData(UAV_REGISTER::u0);
	m_pShadedBuffer->Update_RWData(UAV_REGISTER::u1);
}

CParticle* CParticle::Create()
{
	CParticle* pInstnace = new CParticle;
	if (FAILED(pInstnace->Ready_Particle()))
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
