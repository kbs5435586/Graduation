#include "framework.h"
#include "CollisionMgr.h"
#include "Management.h"
#include "Player.h"
#include "NPC.h"
#include "Collider.h"
#include "Transform.h"
_IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{

}

HRESULT CCollisionMgr::Ready_CollsionManager()
{
	return S_OK;
}

void CCollisionMgr::Update_CollisionManager(const _float& fTimeDelta)
{
	Player_to_NPC_Collision();
	Throw_to_NPC_Collision();
	Skill_to_NPC_Collision(fTimeDelta);
}

void CCollisionMgr::Player_to_NPC_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{

			if (dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_OBB"))
				->Collision_OBB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_OBB"))))
			{
				if (iter0->GetIsHit())
				{
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter1->GetIsParticle() = true;
				}
				else
				{
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					(iter0)->GetOBBCollision() = true;
					iter0->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
				}
				/////////
				if (iter0->GetIsDash())
				{
					(iter1)->GetOBBCollision() = true;
					iter1->GetIsParticle() = true;
					//iter1->GetIsDead() = true;
					//asdasdasd
				}
				

			}

		}
	}

}


void CCollisionMgr::Throw_to_NPC_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Arrow"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			_float fLength = 0.f;

			_vec3 iter0_Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// Arrow
			_vec3 iter1_Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// NPC


			_vec3 vDistance = iter1_Pos - iter0_Pos;

			fLength = vDistance.Length();


			if (fLength <= 5.f)
			{
				(iter1)->GetOBBCollision() = true;
				iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
				iter1->GetIsParticle() = true;
				iter0->GetIsDead() = true;
			}


	/*		if (dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_OBB"))
				->Collision_OBB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_OBB"))))
			{
				if (iter0->GetIsHit())
				{
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter1->GetIsParticle() = true;
				}
				else
				{
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					(iter0)->GetOBBCollision() = true;
					iter0->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
				}


			}*/

		}
	}
}

void CCollisionMgr::Skill_to_NPC_Collision(const _float& fTimeDelta)
{

	if (damage)
	{
		time += fTimeDelta;
		if (time > 1.f)
		{
			time = 0.f;
			damage = false;
		}
	}
	


	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Skill"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
		{
			
			_float fLength = 0.f;

			_vec3 iter0_Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// Fire
			_vec3 iter1_Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// NPC


			_vec3 vDistance = iter1_Pos - iter0_Pos;

			fLength = vDistance.Length();


			if (fLength <= 50.f)
			{
				//(iter1)->GetOBBCollision() = true;
				iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
				iter1->GetIsParticle() = true;
				if (!damage)
				{
					iter1->GetInfo().fHP -= 0.1f;
					damage = true;
				}
				if (iter1->GetInfo().fHP < 0)
					iter1->GetIsDead() = true;
				iter0->GetIsDead() = true;
			}
			

		
		}



	}
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
