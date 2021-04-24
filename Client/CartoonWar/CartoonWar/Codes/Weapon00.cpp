#include "framework.h"
#include "Management.h"
#include "Weapon00.h"

CWeapon00::CWeapon00()
	: CWeapon()
{
}

CWeapon00::CWeapon00(const CWeapon00& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon00::Ready_Prototype()
{
	return S_OK;
}

HRESULT CWeapon00::Ready_GameObject(void* pArg)
{
	return S_OK;
}

_int CWeapon00::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CWeapon00::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CWeapon00::Render_GameObject()
{
}

HRESULT CWeapon00::CreateInputLayout()
{
	return S_OK;
}

void CWeapon00::SetUp_Anim()
{
}

CWeapon00* CWeapon00::Create()
{
	return nullptr;
}

CGameObject* CWeapon00::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	return nullptr;
}

void CWeapon00::Free()
{
}

HRESULT CWeapon00::Ready_Component()
{
	return S_OK;
}
