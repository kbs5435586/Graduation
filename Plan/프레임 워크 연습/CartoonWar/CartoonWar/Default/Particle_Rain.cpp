#include "framework.h"
#include "Particle_Rain.h"
#include "Management.h"

HRESULT CParticle_Rain::Init()
{
	if (FAILED(Ready_Layer_BasicShape(L"Layer_BasicShape", pManagement)))
		return E_FAIL;


	return S_OK;
}

_int CParticle_Rain::Update()
{
	return _int();
}

_int CParticle_Rain::LastUpdate()
{
	return _int();
}

void CParticle_Rain::Render()
{
}

void CParticle_Rain::Free()
{
}
