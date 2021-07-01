#include "framework.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Management.h"
_IMPLEMENT_SINGLETON(CCamera_Manager)


CCamera_Manager::CCamera_Manager()
{
	m_matOrtho = XMMatrixOrthographicLH((_float)WINCX, (_float)WINCY, 0.f, 1.f);
}




void CCamera_Manager::Free()
{
}
