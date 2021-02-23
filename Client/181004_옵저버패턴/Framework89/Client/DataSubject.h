#pragma once
#include "Subject.h"
class CDataSubject :	// �������
	public CSubject
{
	DECLARE_SINGLETON(CDataSubject)

public:
	enum MESSAGE { PLAYER_DATA, PLAYER_MATRIX, MONSTER_DATA, MONSTER_MATRIX, END };

private:
	CDataSubject();
	virtual ~CDataSubject();

public:
	list<void*>* GetDataLst(int iMessage);

public:
	virtual void Release() override;

public:
	void AddData(int iMessage, void* pData);	// �������� ������ ����� �߰�.
	void RemoveData(int iMessage, void* pData);	// ������ ����� ����.

private:
	map<int, list<void*>>	m_MapData;
};

