#include "framework.h"
#include "Management.h"
#include "Rock01.h"

CRock01::CRock01()
{
}

CRock01::CRock01(const CRock01& rhs)
{
}

HRESULT CRock01::Ready_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRock01::Ready_GameObject(void* pArg)
{
	return E_NOTIMPL;
}

_int CRock01::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CRock01::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CRock01::Render_GameObject()
{
}

HRESULT CRock01::CreateInputLayout()
{
	return E_NOTIMPL;
}

CRock01* CRock01::Create()
{
	return nullptr;
}

CGameObject* CRock01::Clone_GameObject(void* pArg)
{
	return nullptr;
}

void CRock01::Free()
{
}

HRESULT CRock01::Ready_Component()
{
	return E_NOTIMPL;
}
