#include "framework.h"
#include "Management.h"
#include "Rock01_C.h"

CRock01_C::CRock01_C()
{
}

CRock01_C::CRock01_C(const CRock01_C& rhs)
{
}

HRESULT CRock01_C::Ready_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRock01_C::Ready_GameObject(void* pArg)
{
	return E_NOTIMPL;
}

_int CRock01_C::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CRock01_C::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CRock01_C::Render_GameObject()
{
}

HRESULT CRock01_C::CreateInputLayout()
{
	return E_NOTIMPL;
}

CRock01_C* CRock01_C::Create()
{
	return nullptr;
}

CGameObject* CRock01_C::Clone_GameObject(void* pArg, const _uint& iIdx )
{
	return nullptr;
}

void CRock01_C::Free()
{
}

HRESULT CRock01_C::Ready_Component()
{
	return E_NOTIMPL;
}
