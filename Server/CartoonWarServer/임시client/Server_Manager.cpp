#include "framework.h"
#include "Server_Manager.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CServer_Manager)
void CServer_Manager::ClientServer()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	// Layer_Basic �ȿ� �ִ� 0��° ��ü�� ����
	CGameObject* pCube = pManagement->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_BasicShape", 0);
	// Layer_Basic �ȿ� �ִ� 0��° ��ü�� Transform�� ����
	CTransform* pTransform_Cube = (CTransform*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_BasicShape", L"Com_Transform", 0);

	_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

	CGameObject* pTerrain = pManagement->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", 0);
	CTransform* pTransform_Terrain = (CTransform*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Terrain", L"Com_Transform", 0);


	Safe_Release(pManagement);
}

void CServer_Manager::Free() // ���⿡ ����, ���� ����
{

}
