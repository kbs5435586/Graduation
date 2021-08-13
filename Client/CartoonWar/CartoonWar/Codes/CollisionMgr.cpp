#include "framework.h"
#include "CollisionMgr.h"
#include "Management.h"
#include "Player.h"
#include "NPC.h"
#include "Collider.h"
#include "Transform.h"
#include "LowPoly.h"
#include "Fire.h"
#include "Throw_Arrow.h"
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
	AABB();
	Range();
	OBB();
	Skill_to_NPC_Collision();
}
void CCollisionMgr::AABB()
{
	Deffend();
	Enviroment_to_Player_Or_NPC();
	Player_to_NPC_Collision();
	NPC_to_NPC_Collision();
}
void CCollisionMgr::OBB()
{
	Player_to_NPC_Attack_Collision();
	Player_to_Deffend_Attack_Collision();
	NPC_to_Deffend_Attack_Collision();
}
void CCollisionMgr::Range()
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
				iter1->GetInfo().fHP -= 1.f;
			}
		}
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend"))
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
				iter1->GetInfo().fHP -= 1.f;
			}
		}
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
		{
			if (dynamic_cast<CThrow_Arrow*>(iter0)->GetOwnPlayer() == iter1)
				continue;

			_float fLength = 0.f;

			_vec3 iter0_Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// Arrow
			_vec3 iter1_Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// NPC


			_vec3 vDistance = iter1_Pos - iter0_Pos;

			fLength = vDistance.Length();


			if (fLength <= 5.f)
			{
				(iter1)->GetOBBCollision() = true;
				_vec3 vTargetPos = *dynamic_cast<CTransform*>(iter1)->Get_StateInfo(CTransform::STATE_POSITION);
				_vec3 vPos = *dynamic_cast<CTransform*>(iter0)->Get_StateInfo(CTransform::STATE_POSITION);
				_vec3 vTemp = { vPos - vTargetPos };
				iter1->GetAttackedObject_Matrix()._41 = vTemp.x;
				iter1->GetAttackedObject_Matrix()._42 = vTemp.y;
				iter1->GetAttackedObject_Matrix()._43 = vTemp.z;
				//iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
				iter1->GetIsParticle() = true;
				iter0->GetIsDead() = true;
				iter1->GetInfo().fHP -= 1.f;
			}
		}
	}
}
//OBB
void CCollisionMgr::Player_to_NPC_Attack_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			if (!iter0->GetIsShow() || !iter1->GetIsShow())
				continue;
			_vec3 vIter0Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 5.f)
				continue;

			if (dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_Attack"))
				->Collision_OBB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_Attack"))))
			{
				if (iter0->GetIsHit())
				{
					(iter1)->GetIsBack() = true;
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter1->GetIsParticle() = true;
					iter0->GetIsHit() = false;
					iter1->GetInfo().fHP -= 1;
				}
				else if (iter1->GetIsHit())
				{
					(iter0)->GetIsBack() = true;
					(iter0)->GetOBBCollision() = true;
					iter0->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter0->GetIsParticle() = true;
					iter1->GetIsHit() = false;
					iter0->GetInfo().fHP -= 1;
				}


			}

		}
	}
}
void CCollisionMgr::Player_to_Deffend_Attack_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend"))
		{
			_vec3 vIter0Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 5.f)
				continue;

			if (dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_Attack"))
				->Collision_OBB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_OBB"))))
			{
				if (iter0->GetIsHit())
				{
					(iter1)->GetIsBack() = true;
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter1->GetIsParticle() = true;
					iter0->GetIsHit() = false;
					iter1->GetInfo().fHP -= 1;
				}
				else if (iter1->GetIsHit())
				{
					(iter0)->GetIsBack() = true;
					(iter0)->GetOBBCollision() = true;
					iter0->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter0->GetIsParticle() = true;
					iter1->GetIsHit() = false;
					iter0->GetInfo().fHP -= 1;
				}


			}

		}
	}
}
void CCollisionMgr::NPC_to_Deffend_Attack_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend"))
		{

			_vec3 vIter0Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 5.f)
				continue;

			if (dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_Attack"))
				->Collision_OBB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_OBB"))))
			{
				if (iter0->GetIsHit())
				{
					(iter1)->GetIsBack() = true;
					(iter1)->GetOBBCollision() = true;
					iter1->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter1->GetIsParticle() = true;
					iter0->GetIsHit() = false;
					iter1->GetInfo().fHP -= 1;
				}
				else if (iter1->GetIsHit())
				{
					(iter0)->GetIsBack() = true;
					(iter0)->GetOBBCollision() = true;
					iter0->GetAttackedObject_Matrix() = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_Matrix();
					iter0->GetIsParticle() = true;
					iter1->GetIsHit() = false;
					iter0->GetInfo().fHP -= 1;
				}


			}

		}
	}

}

void CCollisionMgr::Skill_to_NPC_Collision()
{	
	
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire"))
	{
		if (dynamic_cast<CFire*>(iter0)->GetSCheck())
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
					dynamic_cast<CFire*>(iter0)->SetDCheck(true);
					iter1->GetIsParticle() = true;
					_bool Damage = dynamic_cast<CFire*>(iter0)->GetDamage();

					if (Damage)
					{
						iter1->GetInfo().fHP -= 1.f;
						dynamic_cast<CFire*>(iter0)->SetDamage(false);
					}

					if (iter1->GetInfo().fHP < 0)
						iter1->GetIsDead() = true;
				}
				else
				{
					dynamic_cast<CFire*>(iter0)->SetDCheck(false);
				}
			}			
		}		
	}
}

//void CCollisionMgr::Teleport_to_NPC_Collision(const _float& fTimeDelta)
//{
//
//
//	_bool one{};
//	_bool two{};
//	CGameObject* oneP{};
//	CGameObject* twoP{};
//	//리스트로 포문을 돌리지 말고 각각 받아와서 체크
//	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Teleport"))
//	{
//		//if (dynamic_cast<CFire*>(iter0)->getfirend() == 0)
//		//{
//		//	one = true;
//		//	oneP = iter0;
//		//}
//		//if (dynamic_cast<CFire*>(iter0)->getfirend() == 1)
//		//{
//		//	two = true;
//		//	twoP = iter0;
//		//}
//	}
//	//if(one)
//	//	oneP = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Teleport", 0);
//	//if(two)
//	//	twoP = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Teleport", 1);
//	_bool yeah{};
//	if (one && two)
//	{
//		for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Teleport"))
//		{
//			for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
//			{
//				_float fLength = 0.f;
//
//				_vec3 iter0_Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// Fire
//				_vec3 iter1_Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);// NPC
//
//				_vec3 vDistance = iter1_Pos - iter0_Pos;
//
//				fLength = vDistance.Length();
//
//
//				if (fLength <= 30.f)
//				{
//					iter1->GetIsParticle() = true;
//					if (oneP == iter0)
//					{
//						_vec3 tttt = *dynamic_cast<CTransform*>(twoP->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
//						*dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION) = tttt + vDistance;
//							
//						
//						oneP->GetIsDead() = true;
//						twoP->GetIsDead() = true;
//						//iter0->GetIsDead() = true;
//						yeah = true;
//					}
//					if (twoP == iter0)
//					{
//						_vec3 tttt = *dynamic_cast<CTransform*>(oneP->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
//						*dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION) = tttt + vDistance;
//						//*dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION) =
//						//	*dynamic_cast<CTransform*>(oneP->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION)+ vDistance;
//						
//						oneP->GetIsDead() = true;
//						twoP->GetIsDead() = true;
//						yeah = true;
//						//iter0->GetIsDead() = true;
//					}								
//				}
//			}
//			if (yeah)
//				break;
//
//		}
//	}
//}
//AABB

void CCollisionMgr::Deffend()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend"))
		{
			if (iter0 == iter1)
				continue;
			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 10.f)
				continue;

			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
		{
			if (iter0 == iter1)
				continue;
			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 10.f)
				continue;

			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			if (iter0 == iter1)
				continue;
			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 10.f)
				continue;

			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
	}
}
void CCollisionMgr::Enviroment_to_Player_Or_NPC()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Lowpoly"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
		{
			//if (iter0->GetEnviType() == ENVITYPE::ENVI_FLOWER || iter0->GetEnviType() == ENVITYPE::ENVI_PLANT)
			//	continue;

			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();

			if (fLen >= 50.f)
				continue;

 			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			//if (iter0->GetEnviType() == ENVITYPE::ENVI_FLOWER || iter0->GetEnviType() == ENVITYPE::ENVI_PLANT)
			//	continue;

			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();

			if (fLen >= 50.f)
				continue;

			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
	}
}
void CCollisionMgr::Player_to_NPC_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();

			if (fLen >= 4.f)
				continue;

			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
		}
		for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
		{
			for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
			{
				if (iter0 == iter1)
					continue;
				CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
				CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

				_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
				_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);

				_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
				_float fLen = vLenTemp.Length();
				if (fLen >= 4.f)
					continue;

				dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
					->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
			}
		}

	}

}
void CCollisionMgr::NPC_to_NPC_Collision()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
		{
			if (iter0 == iter1)
				continue;
			CTransform* pIter0Transform = dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"));
			CTransform* pIter1Transform = dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"));

			_vec3 vIter0Pos = *pIter0Transform->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vIter1Pos = *pIter1Transform->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float fLen = vLenTemp.Length();
			if (fLen >= 4.f)
				continue;
			dynamic_cast<CCollider*>(iter0->Get_ComponentPointer(L"Com_Collider_AABB"))
				->Collision_AABB(dynamic_cast<CCollider*>(iter1->Get_ComponentPointer(L"Com_Collider_AABB")), pIter0Transform, pIter1Transform);
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
