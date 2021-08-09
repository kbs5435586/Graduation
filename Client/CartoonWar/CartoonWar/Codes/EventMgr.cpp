#include "framework.h"
#include "Management.h"
#include "EventMgr.h"
#include "Player.h"
#include "NPC.h"
#include "Flag.h"
#include "UI_OnHead_Gage.h"

_IMPLEMENT_SINGLETON(CEventMgr)
CEventMgr::CEventMgr()
{
}

HRESULT CEventMgr::Ready_EventMgr()
{
	return S_OK;
}

void CEventMgr::Update_EventMgr()
{
	Access_Flag_Player();
}

void CEventMgr::Access_Flag_Player()
{
    for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player"))
    {
        for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Flag"))
        {
            _vec3   vIter0Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
            _vec3   vIter1Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3	vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float	fLen = vLenTemp.Length();

			if (fLen <= 10.f)
			{
				if (!iter0->GetIsCheckRange_Flag())
				{
					if (iter0->GetCurTeam() == TEAM::TEAM_BLUE)
					{
						dynamic_cast<CFlag*>(iter1)->m_iBlueCnt++;
					}
					else if (iter0->GetCurTeam() == TEAM::TEAM_RED)
					{
						dynamic_cast<CFlag*>(iter1)->m_iRedCnt++;
					}
					iter0->GetIsCheckRange_Flag() = true;
				}

			}
			else if (fLen > 20.f && fLen<=40.f)
			{
				if (iter0->GetIsCheckRange_Flag())
				{
					if (iter0->GetCurTeam() == TEAM::TEAM_BLUE)
					{
						dynamic_cast<CFlag*>(iter1)->m_iBlueCnt--;
					}
					else if (iter0->GetCurTeam() == TEAM::TEAM_RED)
					{
						dynamic_cast<CFlag*>(iter1)->m_iRedCnt--;
					}
					iter0->GetIsCheckRange_Flag() = false;
				}
			}


		}
	}

}

void CEventMgr::Access_Flag_NPC()
{
	for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC"))
	{
		for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Flag"))
		{
			_vec3	vIter0Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3	vIter1Pos = *dynamic_cast<CTransform*>(iter1->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3	vLenTemp = Vector3_::Subtract(vIter0Pos, vIter1Pos);
			_float	fLen = vLenTemp.Length();
			if (fLen >= 10.f)
				continue;
			if (iter0->GetCurTeam() == TEAM::TEAM_BLUE)
			{

			}
			else if (iter0->GetCurTeam() == TEAM::TEAM_RED)
			{

			}
		}
	}
}

void CEventMgr::Free()
{
}
