#include "framework.h"
#include "Static_Mesh.h"

HRESULT CStatic_Mesh::Ready_Component()
{
	return S_OK;
}

void CStatic_Mesh::Render_Mesh()
{
}

CComponent* CStatic_Mesh::Clone_Component(void* pArg)
{
	return nullptr;
}

void CStatic_Mesh::Free()
{
}
