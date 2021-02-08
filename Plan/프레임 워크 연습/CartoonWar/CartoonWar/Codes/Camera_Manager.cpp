#include "framework.h"
#include "Camera_Manager.h"
_IMPLEMENT_SINGLETON(CCamera_Manager)


CCamera_Manager::CCamera_Manager()
{
	//m_matOrtho = Matrix::CreateOrthographic();
}

void CCamera_Manager::Free()
{
}
