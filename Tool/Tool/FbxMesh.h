#pragma once
#include "Component.h"
class CFbxMesh :
	public CComponent
{
private:
	CFbxMesh(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFbxMesh(const CFbxMesh& rhs);
	virtual ~CFbxMesh() = default;
};

