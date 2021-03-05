#include "framework.h"
#include "Component.h"

CComponent::CComponent()
{

}

CComponent::CComponent(const CComponent& rhs)
{

}

HRESULT CComponent::Ready_Component()
{
	return S_OK;
}

void CComponent::Free()
{

}
