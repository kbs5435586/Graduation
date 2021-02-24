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

	//��ƼŬ�� ����� �ؽ��� �ε�
	//
	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	

	//��ƼŬ �ý��� �ʱ�ȭ
	if (FAILED(Initialize()))
		return E_FAIL;

	//���� �� �ε��� ���� ����, ����ִ�ä��
	m_pBufferCom = (CBuffer_RectCol*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_SphereCol");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return false;
}

void CParticle::Release()
{
	//���� ����

	//��ƼŬ �ý��� ����

	//��ƼŬ�� ����� �ؽ��� ����


}

bool CParticle::Frame()
{

	//������ ��ƼŬ ����

	//�� ��ƼŬ ����

	//��ƼŬ ������Ʈ

	//��ƼŬ �������ۿ� ����


	return false;
}

void CParticle::Render()
{
	//���� �� �ε������� �׷��� ���������ο� �־� �׸� �غ�
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
