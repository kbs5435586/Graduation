#pragma once
class CAstarMgr
{
	DECLARE_SINGLETON(CAstarMgr)

private:
	CAstarMgr();
	~CAstarMgr();

public:
	list<TILE*>& GetBestLst() { return m_BestLst; }

public:
	void StartAstar(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vGoal);

private:
	bool SearchRoute(int iStartIndex, int iGoalIndex);
	void MakeBestLst(int iStartIndex, int iGoalIndex);

private:
	int GetTileIndex(const D3DXVECTOR3& vPos);
	bool IsPicking(const D3DXVECTOR3& vPos, int iIndex);

private:
	bool CheckOpenLst(int iIndex);
	bool CheckCloseLst(int iIndex);

private:
	list<int>	m_OpenLst;
	list<int>	m_CloseLst;
	list<TILE*>	m_BestLst;	// °æ·Î

	int m_iStartIndex;
};

