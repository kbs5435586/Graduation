#include <iostream>
using namespace std;

// 브릿지를 사용하지 않은 일반적인 경우
class CUnit
{
public:
	virtual void Attack() = 0;
};

class CHorseRider : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "말을 탄다" << endl;
	}
};

class CHorseBow : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "활을 들고 싸운다" << endl;
	}
};

class CHorseSpear : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "창을 들고 싸운다" << endl;
	}
};

class CHorseSword : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "검을 들고 싸운다" << endl;
	}
};

class CSoldier : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "뚜벅이" << endl;
	}
};

class CLancer : public CSoldier
{
public:
	virtual void Attack()
	{
		CSoldier::Attack();
		cout << "창을 들고 싸운다" << endl;	// 여기서 문제!
	}
};

void main()
{
	CUnit* pUnit1 = new CHorseBow;
	CUnit* pUnit2 = new CHorseSpear;
	CUnit* pUnit3 = new CHorseSword;

	pUnit1->Attack();

	cout << "------------------------" << endl;

	pUnit2->Attack();

	cout << "------------------------" << endl;

	pUnit3->Attack();

	delete pUnit1;
	delete pUnit2;
	delete pUnit3;
}