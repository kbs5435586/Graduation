#include "framework.h"
#include "Particle.h"
#include "Management.h"

CParticle::CParticle():
	m_vertices{}, m_VertexBuffer{}, m_IdxBuffer{}
{

}

CParticle::~CParticle()
{
}

bool CParticle::Init()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();

	//파티클에 사용할 텍스쳐 로드
	//
	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	

	//파티클 시스템 초기화
	if (FAILED(Initialize()))
		return E_FAIL;

	//정점 및 인덱스 버퍼 생성, 비어있는채로
	m_pBufferCom = (CBuffer_RectCol*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_SphereCol");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return false;
}

void CParticle::Release()
{
	//버퍼 해제

	//파티클 시스템 해제

	//파티클에 사용한 텍스쳐 해제


}

bool CParticle::Frame()
{

	//오래된 파티클 해제

	//새 파티클 방출

	//파티클 업데이트

	//파티클 정점버퍼에 갱신


	return false;
}

void CParticle::Render()
{
	//정점 및 인덱스버퍼 그래픽 파이프라인에 넣어 그릴 준비
}

bool CParticle::Initialize()
{
	int i;


	// Set the random deviation of where the particles can be located when emitted.
	m_Deviation.x = 0.5f;
	m_Deviation.y = 0.1f;
	m_Deviation.z = 2.0f;

	// Set the speed and speed variation of particles.
	m_ParticleVelocity = 1.0f;
	m_ParticleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_ParticleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_ParticlePerSec = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_MaxParticle = 5000;

	// Create the particle list.
	//m_particleList = new ParticleType[m_maxParticles];
	//if (!m_particleList)
	//{
	//	return false;
	//}

	// Initialize the particle list.
	//for (i = 0; i < m_maxParticles; i++)
	//{
	//	m_particleList[i].active = false;
	//}

	// Initialize the current particle count to zero since none are emitted yet.
	m_CurrentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_AccumulatedTime = 0.0f;

	return true;
}
