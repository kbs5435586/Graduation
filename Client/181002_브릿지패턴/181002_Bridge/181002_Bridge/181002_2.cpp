#include <iostream>
using namespace std;

// 브릿지 패턴을 적용했을 때

// 구현층(Implementor)
class CAttackMotion
{
public:
	virtual void Motion() = 0;
};

class CAttackBow : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "활을 들고 싸움" << endl;
	}
};

class CAttackSpear : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "창을 들고 싸움" << endl;
	}
};

class CAttackSword : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "검을 들고 싸움" << endl;
	}
};

// 추상층(Abstraction)
class CUnit
{
public:
	virtual void Attack() = 0;
	
public:
	void Create(CAttackMotion* pMotion)
	{
		m_pMotion = pMotion;
	}

protected:
	CAttackMotion*	m_pMotion;
};

class CHorseRider : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "말을 탄다" << endl;
		m_pMotion->Motion();
	}
};

class CSoldier : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "뚜벅이" << endl;
		m_pMotion->Motion();
	}
};

void main()
{
	// 브릿지 패턴은 추상층과 구현층을 분리 시키고 이둘의 결합을 동적으로 수행한다.
	// 따라서, 확장설계가 용이해진다.


	// 구현층
	CAttackMotion* pAttackBow = new CAttackBow;
	CAttackMotion* pAttackSpear = new CAttackSpear;
	CAttackMotion* pAttackSword = new CAttackSword;


	// 추상층
	CUnit* pHorseRider = new CHorseRider;
	pHorseRider->Create(pAttackBow);
	pHorseRider->Attack();

	cout << "------------------------------------" << endl;

	pHorseRider->Create(pAttackSpear);
	pHorseRider->Attack();

	cout << "------------------------------------" << endl;

	pHorseRider->Create(pAttackSword);
	pHorseRider->Attack();

	CUnit* pSoldier = new CSoldier;
	pSoldier->Create(pAttackBow);
	pSoldier->Attack();

	cout << "------------------------------------" << endl;

	pSoldier->Create(pAttackSpear);
	pSoldier->Attack();

	cout << "------------------------------------" << endl;

	pSoldier->Create(pAttackSword);
	pSoldier->Attack();

	delete pSoldier;
	delete pHorseRider;
	delete pAttackSpear;
	delete pAttackBow;
	delete pAttackSword;
}