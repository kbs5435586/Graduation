#include "framework.h"
#include "UI.h"

CUI::CUI()
	: CGameObject()
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

CGameObject* CUI::Clone_GameObject(void* pArg)
{
	return nullptr;
}

void CUI::Free()
{

	CGameObject::Free();
}
