#pragma once
class CObserver	// 관찰자
{
public:
	CObserver();
	virtual ~CObserver();

public:
	// subject로부터 데이터를 알림받고 해당 옵저버가 필요한 객체들에게 데이터 갱신
	virtual void Update(int iMessage, void* pData)	PURE;
};

