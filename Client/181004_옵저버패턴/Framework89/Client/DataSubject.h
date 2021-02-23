#pragma once
#include "Subject.h"
class CDataSubject :	// 관찰대상
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
	void AddData(int iMessage, void* pData);	// 옵저버가 관찰할 대상을 추가.
	void RemoveData(int iMessage, void* pData);	// 관찰할 대상을 제거.

private:
	map<int, list<void*>>	m_MapData;
};

