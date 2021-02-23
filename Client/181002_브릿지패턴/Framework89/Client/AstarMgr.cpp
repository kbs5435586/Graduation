#include "stdafx.h"
#include "AstarMgr.h"
#include "Terrain.h"

IMPLEMENT_SINGLETON(CAstarMgr)

CAstarMgr::CAstarMgr()
	: m_iStartIndex(0)
{
}


CAstarMgr::~CAstarMgr()
{
}

void CAstarMgr::StartAstar(const D3DXVECTOR3 & vStart, const D3DXVECTOR3 & vGoal)
{
	CObj* pTerrain = GET_INSTANCE(CObjMgr)->GetTerrain();
	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->GetTile();

	for (auto& pTile : vecTile)
	{
		if (0 == pTile->byOption)
			pTile->byDrawID = 2;
	}

	// �Ź� ���� ã�� ������ ������ ������� ���� ����.
	m_OpenLst.clear();
	m_CloseLst.clear();
	m_BestLst.clear();

	m_iStartIndex = GetTileIndex(vStart);
	int iGoalIndex = GetTileIndex(vGoal);

	if (0 > m_iStartIndex || 0 > iGoalIndex)
		return;

	if (m_iStartIndex == iGoalIndex)
		return;

	if (0 != vecTile[iGoalIndex]->byOption)
		return;

	if (true == SearchRoute(m_iStartIndex, iGoalIndex))
		MakeBestLst(m_iStartIndex, iGoalIndex);
}

bool CAstarMgr::SearchRoute(int iStartIndex, int iGoalIndex)
{
	CObj* pTerrain = GET_INSTANCE(CObjMgr)->GetTerrain();
	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->GetTile();
	vector<list<TILE*>>& vecGraph = dynamic_cast<CTerrain*>(pTerrain)->GetGraph();

	if (!m_OpenLst.empty())
		m_OpenLst.pop_front();

	// ���� iStartIndex�� ���������� Close�� �����Ͽ� �˻� ��󿡼� ����.
	m_CloseLst.push_back(iStartIndex);

	// ���� �׷����� ����..
	for (auto& pTile : vecGraph[iStartIndex])
	{
		// ã�� ���� �������̶�� return
		if (iGoalIndex == pTile->iIndex)
		{
			pTile->iParentIndex = iStartIndex;
			return true;	// ��� ����
		}

		if (true == CheckOpenLst(pTile->iIndex) && true == CheckCloseLst(pTile->iIndex))
		{
			pTile->iParentIndex = iStartIndex;
			m_OpenLst.push_back(pTile->iIndex);
		}
	}

	if (m_OpenLst.empty())
		return false; // ��� ����

	// �޸���ƽ�� �����Ѵ�. (��� ����) ��� = ��������� �Ÿ� + ���������� �Ÿ�
	int iOriginStart = m_iStartIndex;

	m_OpenLst.sort([&vecTile, &iOriginStart, &iGoalIndex](int iIndexA, int iIndexB)
	{
		D3DXVECTOR3 vDir1 = vecTile[iOriginStart]->vPos - vecTile[iIndexA]->vPos;
		D3DXVECTOR3 vDir2 = vecTile[iGoalIndex]->vPos - vecTile[iIndexA]->vPos;

		D3DXVECTOR3 vDir3 = vecTile[iOriginStart]->vPos - vecTile[iIndexB]->vPos;
		D3DXVECTOR3 vDir4 = vecTile[iGoalIndex]->vPos - vecTile[iIndexB]->vPos;

		float fDist1 = D3DXVec3Length(&vDir1) + D3DXVec3Length(&vDir2);
		float fDist2 = D3DXVec3Length(&vDir3) + D3DXVec3Length(&vDir4);

		return fDist1 < fDist2;
	});

	return SearchRoute(m_OpenLst.front(), iGoalIndex);
}

void CAstarMgr::MakeBestLst(int iStartIndex, int iGoalIndex)
{
	CObj* pTerrain = GET_INSTANCE(CObjMgr)->GetTerrain();
	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->GetTile();

	vecTile[iGoalIndex]->byDrawID = 36;
	m_BestLst.push_front(vecTile[iGoalIndex]);
	int iRootIdx = vecTile[iGoalIndex]->iParentIndex;

	while (true)
	{
		if (iStartIndex == iRootIdx)
			break;

		vecTile[iRootIdx]->byDrawID = 36;
		m_BestLst.push_front(vecTile[iRootIdx]);
		iRootIdx = vecTile[iRootIdx]->iParentIndex;
	}
}

int CAstarMgr::GetTileIndex(const D3DXVECTOR3 & vPos)
{
	CObj* pTerrain = GET_INSTANCE(CObjMgr)->GetTerrain();
	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->GetTile();

	for (size_t iIndex = 0; iIndex < vecTile.size(); ++iIndex)
	{
		if (IsPicking(vPos, iIndex))
			return iIndex;
	}

	// ��ã��.
	return -1;
}

bool CAstarMgr::IsPicking(const D3DXVECTOR3& vPos, int iIndex)
{
	CObj* pTerrain = GET_INSTANCE(CObjMgr)->GetTerrain();
	vector<TILE*>& vecTile = dynamic_cast<CTerrain*>(pTerrain)->GetTile();

	// ��ī��Ʈ ����
	// �������� 4���� �������� �ð�������� ���Ѵ�.
	D3DXVECTOR3 vPoint[4] =
	{
		/*12��*/
		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y + TILECY / 2, 0.f },
		/*3��*/
		{ vecTile[iIndex]->vPos.x + TILECX / 2, vecTile[iIndex]->vPos.y, 0.f },
		/*6��*/
		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y - TILECY / 2, 0.f },
		/*9��*/
		{ vecTile[iIndex]->vPos.x - TILECX / 2, vecTile[iIndex]->vPos.y, 0.f },
	};

	// 4���� �������� ���� ���׺��͸� �ð�������� ���Ѵ�.
	D3DXVECTOR3 vDir[4] =
	{
		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]
	};

	// �� �������͸� ���Ѵ�
	D3DXVECTOR3 vNormal[4] =
	{
		{ -vDir[0].y, vDir[0].x, 0.f },
		{ -vDir[1].y, vDir[1].x, 0.f },
		{ -vDir[2].y, vDir[2].x, 0.f },
		{ -vDir[3].y, vDir[3].x, 0.f }
	};

	// ���콺 ��ǥ���������� �� ������ ���⺤�͸� ���Ѵ�.
	D3DXVECTOR3 vMouseDir[4] =
	{
		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	// vMouseDir�� vNormal�� �����Ͽ� ��� ����(�а�)�� ������ true!
	for (int i = 0; i < 4; ++i)
	{
		if (0.f <= D3DXVec3Dot(&vMouseDir[i], &vNormal[i]))
			return false;
	}

	return true;
}

bool CAstarMgr::CheckOpenLst(int iIndex)
{
	for (int iOpenIdx : m_OpenLst)
	{
		if (iIndex == iOpenIdx)
			return false;
	}

	return true;
}

bool CAstarMgr::CheckCloseLst(int iIndex)
{
	for (int iCloseIdx : m_CloseLst)
	{
		if (iIndex == iCloseIdx)
			return false;
	}

	return true;
}
