#include "framework.h"
#include "Management.h"
#include "Rock01_B.h"

CRock01_B::CRock01_B()
{
}

CRock01_B::CRock01_B(const CRock01_B& rhs)
{
}

HRESULT CRock01_B::Ready_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRock01_B::Ready_GameObject(void* pArg)
{
	return E_NOTIMPL;
}

_int CRock01_B::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CRock01_B::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CRock01_B::Render_GameObject()
{
}

HRESULT CRock01_B::CreateInputLayout()
{
	return E_NOTIMPL;
}

CRock01_B* CRock01_B::Create()
{
	return nullptr;
}

CGameObject* CRock01_B::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	return nullptr;
}

void CRock01_B::Free()
{
}

HRESULT CRock01_B::Ready_Component()
{
	return E_NOTIMPL;
}
