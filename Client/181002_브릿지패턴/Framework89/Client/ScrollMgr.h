#pragma once
class CScrollMgr
{
public:
	CScrollMgr();
	~CScrollMgr();

public:
	static D3DXVECTOR3 GetScroll() { return m_vScroll; }

public:
	static void SetScroll(const D3DXVECTOR3& vScroll)
	{
		m_vScroll += vScroll;
	}

private:
	static D3DXVECTOR3 m_vScroll;
};

