#pragma once
class CObserver	// ������
{
public:
	CObserver();
	virtual ~CObserver();

public:
	// subject�κ��� �����͸� �˸��ް� �ش� �������� �ʿ��� ��ü�鿡�� ������ ����
	virtual void Update(int iMessage, void* pData)	PURE;
};

