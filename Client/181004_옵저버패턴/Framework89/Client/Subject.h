#pragma once

class CObserver;
class CSubject
{
public:
	CSubject();
	virtual ~CSubject();

public:
	virtual void Release();

public:
	void Subscribe(CObserver* pObserver);	// 관찰자 등록
	void UnSubscribe(CObserver* pObserver); // 등록 해지
	void Notify(int iMessage, void* pData = nullptr);	// 관찰자들에게 알림

protected:
	list<CObserver*>	m_ObserverLst;
};

