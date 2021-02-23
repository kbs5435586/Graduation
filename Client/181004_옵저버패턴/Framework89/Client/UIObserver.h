#pragma once
#include "Observer.h"
class CUIObserver :
	public CObserver
{
public:
	CUIObserver();
	virtual ~CUIObserver();

public:
	const STAT& GetStat() const { return m_tStat; }

public:
	// CObserver��(��) ���� ��ӵ�
	virtual void Update(int iMessage, void * pData) override;

private:
	STAT	m_tStat;
};

