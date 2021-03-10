#include "framework.h"
#include "Management.h"
#include "Rock01_D.h"

CRock01_D::CRock01_D()
{
}

CRock01_D::CRock01_D(const CRock01_D& rhs)
{
}

HRESULT CRock01_D::Ready_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRock01_D::Ready_GameObject(void* pArg)
{
	return E_NOTIMPL;
}

_int CRock01_D::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CRock01_D::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CRock01_D::Render_GameObject()
{
}

HRESULT CRock01_D::CreateInputLayout()
{
	return E_NOTIMPL;
}

CRock01_D* CRock01_D::Create()
{
	return nullptr;
}

CGameObject* CRock01_D::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	return nullptr;
}

void CRock01_D::Free()
{
}

HRESULT CRock01_D::Ready_Component()
{
	return E_NOTIMPL;
}
