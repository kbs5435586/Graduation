#include "framework.h"
#include "..\Headers\Weapon.h"
#include "Management.h"
#include "StructedBuffer.h"

CWeapon::CWeapon()
	: CGameObject()
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CGameObject(rhs)
{
}