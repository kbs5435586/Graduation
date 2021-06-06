#include "framework.h"
#include "CollisionMgr.h"
_IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{

}

HRESULT CCollisionMgr::Ready_CollsionManager()
{
	return S_OK;
}

void CCollisionMgr::Update_CollisionManager()
{

}

CCollisionMgr* CCollisionMgr::Create()
{
	CCollisionMgr* pInstance = new CCollisionMgr();
	if (FAILED(pInstance->Ready_CollsionManager()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollisionMgr::Free()
{
}
