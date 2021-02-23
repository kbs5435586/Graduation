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
	void Subscribe(CObserver* pObserver);	// ������ ���
	void UnSubscribe(CObserver* pObserver); // ��� ����
	void Notify(int iMessage, void* pData = nullptr);	// �����ڵ鿡�� �˸�

protected:
	list<CObserver*>	m_ObserverLst;
};

